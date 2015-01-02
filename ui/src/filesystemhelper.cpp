/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2015 Dmitriy Purgin <dpurgin@gmail.com>

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

#include "filesystemhelper.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QThreadPool>

#include "filerelocationworker.h"

FileSystemHelper::FileSystemHelper(QObject *parent) :
    QObject(parent),
    mBusy(false),
    mProgress(0),
    mTotalCount(0)
{
}

bool FileSystemHelper::dirIsEmpty(const QString& dirPath) const
{
    return QFileInfo(dirPath).isDir() &&
            (QDir(dirPath).entryList(QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot).size() == 0);
}

bool FileSystemHelper::exists(const QString& filePath) const
{
    return QFileInfo(filePath).exists();
}

QStringList FileSystemHelper::fileList(const QString& dirPath) const
{
    return (QFileInfo(dirPath).isDir()? QDir(dirPath).entryList(QDir::Files): QStringList());
}

bool FileSystemHelper::isRemovable(const QString& filePath) const
{
    QFileInfo fi(filePath);

    return fi.isWritable() && (!fi.isDir() || dirIsEmpty(filePath));
}

bool FileSystemHelper::isWritable(const QString& filePath) const
{
    QFileInfo fi(filePath);

    return fi.exists() && fi.isWritable();
}

bool FileSystemHelper::mkpath(const QString& dirPath) const
{
    return QDir().mkpath(dirPath);
}

bool FileSystemHelper::relocate(const QString& oldPath, const QString& newPath)
{
    bool result = false;

    FileRelocationWorker* worker = new FileRelocationWorker(oldPath, newPath);

    connect(worker, SIGNAL(started()),
            this, SLOT(onWorkerStarted()));
    connect(worker, SIGNAL(finished()),
            this, SLOT(onWorkerFinished()));

    connect(worker, SIGNAL(totalCountChanged(int)),
            this, SLOT(setTotalCount(int)));
    connect(worker, SIGNAL(progressChanged(int)),
            this, SLOT(setProgress(int)));

    if (worker)
        result = QThreadPool::globalInstance()->tryStart(worker);

    return result;
}

bool FileSystemHelper::remove(const QString& filePath) const
{
    QFileInfo fi(filePath);

    return isRemovable(filePath) && (fi.isDir()? QDir().rmdir(filePath): QFile(filePath).remove());
}

bool FileSystemHelper::rename(const QString& filePath, const QString& newName) const
{
    QFile f(filePath);

    return (f.exists() && f.rename(newName));
}

bool FileSystemHelper::sdCardExists() const
{
    return QDir("/media/sdcard").entryList(QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot).size() > 0;
}

QString FileSystemHelper::sdCardPath() const
{
    QFileInfoList fiList = QDir("/media/sdcard").entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    return fiList.size() > 0? fiList.first().absoluteFilePath(): QString();
}
