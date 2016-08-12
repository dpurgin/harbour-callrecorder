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

#include <QObject>
#include <QFileInfo>
#include <QRunnable>

#include "backuphelper.h"

class archive;
class archive_entry;

class BackupWorker : public QObject, public QRunnable
{
    Q_OBJECT

public:
    enum class Mode
    {
        Backup,
        Restore
    };

public:
    BackupWorker(Mode mode, QString fileName, bool compress, QObject* parent = nullptr)
        : QObject(parent),
          QRunnable(),
          mMode(mode),
          mFileName(fileName),
          mCompress(compress)
    {
    }

    virtual ~BackupWorker();

public:
    void run() override;

signals:
    void started();
    void finished(BackupHelper::ErrorCode);
    void progressChanged(int);
    void totalCountChanged(int);

private:
    void writeToArchive(QFileInfo fileInfo, QString pathInArchive = QString());
    void writeToArchive(QString fileName, QString pathInArchive = QString());

private:
    Mode mMode;
    QString mFileName;
    bool mCompress;

    archive* mArchive = nullptr;
    archive_entry* mArchiveEntry = nullptr;
};
