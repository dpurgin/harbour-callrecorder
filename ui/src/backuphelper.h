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

#ifndef HARBOUR_CALLRECORDER_BACKUPHELPER_H
#define HARBOUR_CALLRECORDER_BACKUPHELPER_H

#include <QObject>

class BackupWorker;

class BackupHelper : public QObject
{
    Q_OBJECT
    Q_ENUMS(ErrorCode)
    Q_ENUMS(Operation)

    Q_PROPERTY(QString backupMeta
               READ backupMeta
               NOTIFY backupMetaChanged)

    Q_PROPERTY(bool busy
               READ busy
               NOTIFY busyChanged)

    Q_PROPERTY(ErrorCode errorCode
               READ errorCode
               NOTIFY errorCodeChanged)

    Q_PROPERTY(qint64 estimatedBackupSize
               READ estimatedBackupSize
               NOTIFY estimatedBackupSizeChanged)

    Q_PROPERTY(Operation operation
               READ operation
               NOTIFY operationChanged)

    Q_PROPERTY(int progress
               READ progress
               NOTIFY progressChanged)

    Q_PROPERTY(int totalCount
               READ totalCount
               NOTIFY totalCountChanged)

public:
    enum class ErrorCode
    {
        None,
        FileExists,
        FileNotExists,
        UnableToWrite,
        UnableToStart,
        WrongFileFormat
    };

    enum class Operation
    {
        NotStarted,
        Preparing,
        BackingUp,
        RemovingOldData,
        Restoring,
        Complete
    };


public:
    BackupHelper(QObject* parent = nullptr);
    virtual ~BackupHelper();

    Q_INVOKABLE void backup(const QString& fileName, bool compress, bool overwrite);
    Q_INVOKABLE void estimateBackupSize();
    Q_INVOKABLE void restore(const QString& fileName,
                             const QString& outputLocation,
                             bool removeExisting);
    Q_INVOKABLE void readBackupMeta(const QString& fileName);

    QString backupMeta() const { return mBackupMeta; }
    bool busy() const { return mBusy; }
    ErrorCode errorCode() const { return mErrorCode; }
    qint64 estimatedBackupSize() const { return mEstimatedBackupSize; }
    Operation operation() const { return mOperation; }
    int progress() const { return mProgress; }
    int totalCount() const { return mTotalCount; }

signals:
    void backupMetaChanged(QString);
    void busyChanged(bool);
    void errorCodeChanged(BackupHelper::ErrorCode);
    void estimatedBackupSizeChanged(qint64);
    void operationChanged(BackupHelper::Operation);
    void totalCountChanged(int);
    void progressChanged(int);

private:
    bool tryStartWorker(BackupWorker* worker);

private slots:
    void setBackupMeta(QString backupMeta)
    {
        if (backupMeta != mBackupMeta)
            emit backupMetaChanged(mBackupMeta = backupMeta);
    }

    void setBusy(bool busy)
    {
        if (busy != mBusy)
            emit busyChanged(mBusy = busy);
    }

    void setErrorCode(BackupHelper::ErrorCode errorCode)
    {
        if (errorCode != mErrorCode)
            emit errorCodeChanged(mErrorCode = errorCode);
    }

    void setEstimatedBackupSize(qint64 size)
    {
        if (size != mEstimatedBackupSize)
            emit estimatedBackupSizeChanged(mEstimatedBackupSize = size);
    }

    void setOperation(BackupHelper::Operation operation)
    {
        if (operation != mOperation)
            emit operationChanged(mOperation = operation);
    }

    void setProgress(int progress)
    {
        if (progress != mProgress)
            emit progressChanged(mProgress = progress);
    }

    void setTotalCount(int totalCount)
    {
        if (totalCount != mTotalCount)
            emit totalCountChanged(mTotalCount = totalCount);
    }

private:
    bool mBusy = false;
    int mProgress = -1;
    int mTotalCount = -1;
    qint64 mEstimatedBackupSize = -1;
    QString mBackupMeta;

    ErrorCode mErrorCode = ErrorCode::None;
    Operation mOperation = Operation::NotStarted;
};

Q_DECLARE_METATYPE(BackupHelper::ErrorCode)
Q_DECLARE_METATYPE(BackupHelper::Operation)

QDebug operator<<(QDebug dbg, BackupHelper::ErrorCode errorCode);

#endif // HARBOUR_CALLRECORDER_BACKUPHELPER_H
