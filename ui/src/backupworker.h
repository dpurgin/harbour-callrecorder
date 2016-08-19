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
        Restore,
        EstimateBackupSize,
        ReadBackupMeta
    };

public:
    BackupWorker(QObject* parent = nullptr)
        : QObject(parent),
          QRunnable(),
          mMode(Mode::EstimateBackupSize)
    {
    }

    BackupWorker(QString fileName, QObject* parent = nullptr)
        : QObject(parent),
          QRunnable(),
          mMode(Mode::ReadBackupMeta),
          mFileName(fileName)
    {
    }

    BackupWorker(QString fileName,
                 QString outputLocation,
                 bool removeExisting,
                 QObject* parent = nullptr)
        : QObject(parent),
          QRunnable(),
          mMode(Mode::Restore),
          mFileName(fileName),
          mOutputLocation(outputLocation),
          mRemoveExisting(removeExisting)
    {
    }

    BackupWorker(QString fileName, bool compress, QObject* parent = nullptr)
        : QObject(parent),
          QRunnable(),
          mMode(Mode::Backup),
          mFileName(fileName),
          mCompress(compress)
    {
    }

    virtual ~BackupWorker();

public:
    void run() override;

signals:
    void backupMetaChanged(QString);
    void started();
    void finished(BackupHelper::ErrorCode);
    void operationChanged(BackupHelper::Operation);
    void progressChanged(int);
    void totalCountChanged(int);
    void estimatedBackupSizeChanged(qint64);

private:
    void backup();
    void restore();
    void estimateBackupSize();
    void readBackupMeta();

    void extractFromArchive(QIODevice* device);
    void openArchive();
    void writeToArchive(QFileInfo fileInfo, QString pathInArchive = QString());
    void writeToArchive(QString fileName, QString pathInArchive = QString());
    void writeToArchive(QByteArray data, QString filePathInArchive);

private:
    Mode mMode;
    QString mFileName;
    bool mCompress;
    QString mOutputLocation;
    bool mRemoveExisting = true;

    archive* mArchive = nullptr;
    archive_entry* mArchiveEntry = nullptr;
};
