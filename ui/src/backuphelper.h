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

    Q_ENUMS(ErrorCode)

public:
    BackupHelper(QObject* parent = nullptr);
    virtual ~BackupHelper();

    Q_INVOKABLE void backup(const QString& fileName, bool compress, bool overwrite);
    Q_INVOKABLE void estimateBackupSize();
    Q_INVOKABLE void restore(const QString& fileName);
    Q_INVOKABLE void readBackupMeta(const QString& fileName);

    QString backupMeta() const { return mBackupMeta; }
    bool busy() const { return mBusy; }
    ErrorCode errorCode() const { return mErrorCode; }
    qint64 estimatedBackupSize() const { return mEstimatedBackupSize; }
    int progress() const { return mProgress; }
    int totalCount() const { return mTotalCount; }

signals:
    void backupMetaChanged(QString);
    void busyChanged(bool);
    void errorCodeChanged(ErrorCode);
    void estimatedBackupSizeChanged(qint64);
    void totalCountChanged(int);
    void progressChanged(int);

private:
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

    void setErrorCode(ErrorCode errorCode)
    {
        if (errorCode != mErrorCode)
            emit errorCodeChanged(mErrorCode = errorCode);
    }

    void setEstimatedBackupSize(qint64 size)
    {
        if (size != mEstimatedBackupSize)
            emit estimatedBackupSizeChanged(mEstimatedBackupSize = size);
    }

    bool tryStartWorker(BackupWorker* worker);

private slots:
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
};

QDebug operator<<(QDebug dbg, BackupHelper::ErrorCode errorCode);

#endif // HARBOUR_CALLRECORDER_BACKUPHELPER_H
