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

#include "backuphelper.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QStringBuilder>

#include <archive.h>
#include <archive_entry.h>

#include <libcallrecorder/callrecorderexception.h>
#include <libcallrecorder/libcallrecorder.h>
#include <libcallrecorder/settings.h>

void _writeToArchive(archive* arc, const QFileInfo& fi);
void _writeToArchive(archive* arc, const QString& filePath);

void _writeToArchive(archive* arc, const QString& filePath)
{
    _writeToArchive(arc, QFileInfo(filePath));
}

void _writeToArchive(archive* arc, const QFileInfo& fi)
{
    qDebug() << "adding to archive: " << fi.absoluteFilePath();

    auto* entry = archive_entry_new();

    archive_entry_set_pathname(entry, fi.absoluteFilePath().toUtf8().data());
    archive_entry_set_size(entry, fi.size());
    archive_entry_set_filetype(entry, AE_IFREG);
    archive_entry_set_perm(entry, static_cast< int >(fi.permissions()));

    archive_write_header(arc, entry);

    QFile f(fi.absoluteFilePath());

    if (f.open(QFile::ReadOnly))
    {
        const int bufferSize = 32768;
        char buffer[bufferSize];
        qint64 len = 0;

        while (!f.atEnd())
        {
            len = f.read(buffer, bufferSize);

            archive_write_data(arc, buffer, len);
        }
    }

    archive_entry_free(entry);
}

BackupHelper::BackupHelper(QObject* parent)
    : QObject(parent)
{
}

BackupHelper::~BackupHelper()
{
}

bool BackupHelper::backup(const QString& fileName)
{
    qDebug() << "backing up to " << fileName;

    bool result = false;

    try
    {
        QFileInfo backupFileInfo(fileName);

        if (backupFileInfo.exists())
            throw CallRecorderException(QLatin1String("Backup file exists!"));

        if (!backupFileInfo.absoluteDir().exists())
        {
            if (!QDir().mkpath(backupFileInfo.absolutePath()))
            {
                throw CallRecorderException(QLatin1String("Unable to make path ") %
                                            backupFileInfo.absolutePath());
            }
        }

        QScopedPointer< Settings > settings(new Settings());

        auto fiList = QDir(settings->outputLocation()).entryInfoList(
                    QDir::Files | QDir::Readable, QDir::NoSort);

        auto* arc = archive_write_new();
        archive_write_add_filter_none(arc);
        archive_write_set_format_ustar(arc);

        archive_write_open_filename(arc, fileName.toUtf8().data());

        foreach (QFileInfo fi, fiList)
            _writeToArchive(arc, fi);

        _writeToArchive(arc, LibCallRecorder::databaseFilePath());
        _writeToArchive(arc, LibCallRecorder::settingsFilePath());

        archive_write_close(arc);
        archive_write_free(arc);

        result = true;
    }
    catch (CallRecorderException& e)
    {
        qDebug() << e.qWhat();

        result = false;
    }

    return result;
}

void BackupHelper::restore(const QString&)
{
}
