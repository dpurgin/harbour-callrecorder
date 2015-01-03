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

#include "filerelocationworker.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStringBuilder>

FileRelocationWorker::FileRelocationWorker(const QString& oldPath, const QString& newPath, QObject* parent)
    : QObject(parent),
      QRunnable(),
      mOldPath(oldPath),
      mNewPath(newPath)
{
}

void FileRelocationWorker::run()
{
    emit started();

    QFileInfo newPathFi(mNewPath);

    if (newPathFi.isDir() && newPathFi.exists() && newPathFi.isWritable())
    {
        QFileInfoList fiList = QDir(mOldPath).entryInfoList(QDir::Files);

        int totalCount = fiList.size();

        emit totalCountChanged(totalCount);

        int progressStep = qMax(totalCount / 100, 1);

        QDir newDir(mNewPath);

        int count = 0;
        foreach (QFileInfo fi, fiList)
        {
            QString oldName = fi.absoluteFilePath();
            QString newName = newDir.absoluteFilePath(fi.fileName());

            qDebug() << "moving " << oldName << " to " << newName;

            QFile::rename(oldName, newName);

            if ((++count) % progressStep == 0)
            {
                emit progressChanged(count);
            }
        }
    }

    emit finished();
}
