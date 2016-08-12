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

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QStringBuilder>

#include <archive.h>
#include <archive_entry.h>

#include <libcallrecorder/libcallrecorder.h>
#include <libcallrecorder/settings.h>

#include "backupexception.h"

BackupWorker::~BackupWorker()
{
    qDebug();

    if (mArchiveEntry)
        archive_entry_free(mArchiveEntry);

    if (mArchive)
    {
        if (mMode == Mode::Backup)
            archive_write_free(mArchive);
        else if (mMode == Mode::Restore)
            archive_read_free(mArchive);
    }
}

void BackupWorker::run()
{
    emit started();

    BackupHelper::ErrorCode errorCode = BackupHelper::ErrorCode::None;

    try
    {
        QScopedPointer< Settings > settings(new Settings());

        auto fiList = QDir(settings->outputLocation()).entryInfoList(
                    QDir::Files | QDir::Readable, QDir::NoSort);

        emit totalCountChanged(fiList.size() + 2);

        mArchive = archive_write_new();
        mArchiveEntry = archive_entry_new();

        if (mCompress)
            archive_write_add_filter_bzip2(mArchive);
        else
            archive_write_add_filter_none(mArchive);

        archive_write_set_format_ustar(mArchive);

        int progress = 0;

        archive_write_open_filename(mArchive, mFileName.toUtf8().data());

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

        archive_entry_free(mArchiveEntry);
        mArchiveEntry = nullptr;

        archive_write_close(mArchive);
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

