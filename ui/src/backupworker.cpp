/*
    Call Recorder for SailfishOS
    Copyright (C) 2016 Dmitriy Purgin <dpurgin@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "backupworker.h"

#include <QBuffer>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringBuilder>

#include <archive.h>
#include <archive_entry.h>

#include <libcallrecorder/libcallrecorder.h>
#include <libcallrecorder/settings.h>

#include "backupexception.h"

class ArchiveReadDeleter
{
public:
    static inline void cleanup(archive* pointer)
    {
        qDebug();

        if (pointer)
            archive_read_free(pointer);
    }
};

class ArchiveWriteDeleter
{
public:
    static inline void cleanup(archive* pointer)
    {
        qDebug();

        if (pointer)
            archive_write_free(pointer);
    }
};

class ArchiveEntryDeleter
{
public:
    static inline void cleanup(archive_entry* pointer)
    {
        qDebug();

        if (pointer)
            archive_entry_free(pointer);
    }
};

BackupWorker::~BackupWorker()
{
    qDebug();
}

void BackupWorker::backup()
{
    QScopedPointer< Settings > settings(new Settings());

    auto fiList = QDir(settings->outputLocation()).entryInfoList(
                QDir::Files | QDir::Readable, QDir::NoSort);

    // Update total count for UI progress.
    // fiList.size() is number of recordings.
    // +2 are settings and database file.
    // +1 is for preparing metadata file.
    // +1 is for writing metadata file
    emit totalCountChanged(fiList.size() + 4);

    QScopedPointer< archive, ArchiveWriteDeleter > archiveContainer(
                mArchive = archive_write_new());

    QScopedPointer< archive_entry, ArchiveEntryDeleter > archiveEntryContainer(
                mArchiveEntry = archive_entry_new());

    if (mCompress)
        archive_write_add_filter_bzip2(mArchive);
    else
        archive_write_add_filter_none(mArchive);

    archive_write_set_format_ustar(mArchive);

    int progress = 0;
    qint64 restoreSize = 0;

    archive_write_open_filename(mArchive, mFileName.toUtf8().data());

    // Prepare backup metadata file.
    // This file contains unpacked size and file count
    // The file is written as the first file in archive to speed up
    // reading it from compressed archive
    foreach (auto fi, fiList)
        restoreSize += fi.size();

    restoreSize += QFileInfo(LibCallRecorder::databaseFilePath()).size() +
                   QFileInfo(LibCallRecorder::settingsFilePath()).size();

    emit progressChanged(++progress);

    // Write metadata file as the first file in archive
    // This will speed up loading metadata when restoring

    QJsonObject meta;
    meta.insert("timeStamp", QDateTime::currentDateTime().toString(Qt::ISODate));
    meta.insert("producerVersion", QString(VERSION));
    meta.insert("outputLocation", settings->outputLocation());
    meta.insert("restoreSize", restoreSize);
    meta.insert("totalCount", fiList.size() + 2);

    writeToArchive(QJsonDocument(meta).toJson(), "meta.json");

    emit progressChanged(++progress);

    foreach (auto fi, fiList)
    {
        writeToArchive(fi, "data");

        emit progressChanged(++progress);
    }

    writeToArchive(LibCallRecorder::databaseFilePath());
    emit progressChanged(++progress);

    writeToArchive(LibCallRecorder::settingsFilePath());
    emit progressChanged(++progress);

    qDebug() << "before write archive_write_close()";

    archive_write_close(mArchive);
}

void BackupWorker::estimateBackupSize()
{
    QScopedPointer< Settings > settings(new Settings());

    auto fiList = QDir(settings->outputLocation()).entryInfoList(
                QDir::Files | QDir::Readable, QDir::NoSort);

    qint64 totalSize = 0;

    foreach (auto fi, fiList)
        totalSize += fi.size();

    totalSize += QFileInfo(LibCallRecorder::settingsFilePath()).size() +
                 QFileInfo(LibCallRecorder::databaseFilePath()).size();

    emit estimatedBackupSizeChanged(totalSize);
}

void BackupWorker::estimateRestoreSize()
{
    qDebug();

    qint64 size = 0;

    QScopedPointer< archive, ArchiveReadDeleter > arc(archive_read_new());
    archive_read_support_filter_all(arc.data());
    archive_read_support_format_all(arc.data());

    int result = archive_read_open_filename(arc.data(), mFileName.toUtf8().data(), 32768);

    if (result != ARCHIVE_OK)
        throw BackupException(BackupHelper::ErrorCode::WrongFileFormat, mFileName);

    archive_entry* entry;

    while (archive_read_next_header(arc.data(), &entry) == ARCHIVE_OK)
    {
        qDebug() << archive_entry_pathname(entry);

        size += archive_entry_size(entry);
    }

    qDebug() << "estimated restore size for" << mFileName << size;

    if (size == 0)
    {
        qDebug() << "something went wrong, using archive size as estimate";

        size = QFileInfo(mFileName).size();
    }

    emit estimatedRestoreSizeChanged(size);

}

void BackupWorker::extractFromArchive(QIODevice* device)
{
    qDebug();

    char buffer[32768];

    auto readSize = archive_read_data(mArchive, buffer, sizeof(buffer));

    do
    {
        device->write(buffer, readSize);

        readSize = archive_read_data(mArchive, buffer, sizeof(buffer));
    }
    while (readSize > 0);
}

void BackupWorker::openArchive()
{
    qDebug();

    QScopedPointer< archive, ArchiveReadDeleter > arc(archive_read_new());
    archive_read_support_filter_all(arc.data());
    archive_read_support_format_all(arc.data());

    int result = archive_read_open_filename(arc.data(), mFileName.toUtf8().data(), 32768);

    if (result != ARCHIVE_OK)
        throw BackupException(BackupHelper::ErrorCode::WrongFileFormat, mFileName);

    mArchive = arc.take();
}

void BackupWorker::readBackupMeta()
{
    qDebug();

    bool metaFound = false;

    openArchive();

    QScopedPointer< archive, ArchiveReadDeleter > archiveContainer(mArchive);

    while (archive_read_next_header(mArchive, &mArchiveEntry) == ARCHIVE_OK && !metaFound)
    {
        QString filePath(archive_entry_pathname(mArchiveEntry));

        if (filePath.compare(QLatin1String("meta.json"), Qt::CaseInsensitive) == 0)
        {
            QBuffer buffer;

            buffer.open(QIODevice::ReadWrite);

            extractFromArchive(&buffer);

            qDebug() << buffer.data();

            emit backupMetaChanged(QString::fromUtf8(buffer.data()));

            metaFound = true;
        }
    }

    if (!metaFound)
        throw BackupException(BackupHelper::ErrorCode::WrongFileFormat,
                              QLatin1String("Archive doesn't contain meta.json"));
}

void BackupWorker::restore()
{

}

void BackupWorker::run()
{
    emit started();

    auto errorCode = BackupHelper::ErrorCode::None;

    try
    {
        if (mMode == Mode::Backup)
            backup();
        else if (mMode == Mode::EstimateBackupSize)
            estimateBackupSize();
        else if (mMode == Mode::EstimateRestoreSize)
            estimateRestoreSize();
        else if (mMode == Mode::ReadBackupMeta)
            readBackupMeta();
    }
    catch (BackupException& e)
    {
        qDebug() << e.errorCode() << e.qWhat();

        errorCode = e.errorCode();
    }

    qDebug() << "before finished";

    emit finished(errorCode);

}

void BackupWorker::writeToArchive(QFileInfo fileInfo, QString pathInArchive)
{
    qDebug() << "adding to archive: " << fileInfo.absoluteFilePath();

    mArchiveEntry = archive_entry_clear(mArchiveEntry);

    if (!pathInArchive.isEmpty())
        pathInArchive.append(QLatin1Char('/'));

    pathInArchive.append(fileInfo.fileName());

    archive_entry_set_pathname(mArchiveEntry, pathInArchive.toUtf8().data());
    archive_entry_set_size(mArchiveEntry, fileInfo.size());
    archive_entry_set_filetype(mArchiveEntry, AE_IFREG);
    archive_entry_set_perm(mArchiveEntry, static_cast< int >(fileInfo.permissions()));

    archive_write_header(mArchive, mArchiveEntry);

    QFile f(fileInfo.absoluteFilePath());

    if (f.open(QFile::ReadOnly))
    {
        const int bufferSize = 32768;
        char buffer[bufferSize];
        qint64 len = 0;

        while (!f.atEnd())
        {
            len = f.read(buffer, bufferSize);

            archive_write_data(mArchive, buffer, len);
        }
    }
}

void BackupWorker::writeToArchive(QString fileName, QString pathInArchive)
{
    writeToArchive(QFileInfo(fileName), pathInArchive);
}

void BackupWorker::writeToArchive(QByteArray data, QString filePathInArchive)
{
    mArchiveEntry = archive_entry_clear(mArchiveEntry);

    archive_entry_set_pathname(mArchiveEntry, filePathInArchive.toUtf8().data());
    archive_entry_set_size(mArchiveEntry, data.size());
    archive_entry_set_filetype(mArchiveEntry, AE_IFREG);
    archive_entry_set_perm(mArchiveEntry, 0664);

    archive_write_header(mArchive, mArchiveEntry);

    archive_write_data(mArchive, data.constData(), data.size());
}
