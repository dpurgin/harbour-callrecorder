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

#ifndef HARBOUR_CALLRECORDER_DATABASEREPAIRWORKER_H
#define HARBOUR_CALLRECORDER_DATABASEREPAIRWORKER_H

#include <QThread>

class DatabaseRepairWorker : public QObject
{
    Q_OBJECT
    Q_ENUMS(Operation)
    Q_ENUMS(ErrorCode)
    Q_ENUMS(RepairMode)

    Q_PROPERTY(ErrorCode errorCode
               READ errorCode
               NOTIFY errorCodeChanged)
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
        UnableToStart,
        UnableToRetrieveOrphanedRecords,
        UnableToRemoveOrphanedRecord,
        UnableToRetrieveOrphanedFile,
        UnabletoRemoveOrphanedFile,
        UnableToRestoreOrphanedFile,
        UnhandledException
    };

    enum class Operation
    {
        NotStarted,
        Starting,
        ProcessingOrphanedFiles,
        ProcessingOrphanedRecords,
        Complete
    };

    enum class RepairMode
    {
        Skip,
        Remove,
        Restore
    };

public:
    DatabaseRepairWorker(QObject* parent = nullptr);
    virtual ~DatabaseRepairWorker();

    ErrorCode errorCode() const { return mErrorCode; }
    Operation operation() const { return mOperation; }
    int progress() const { return mProgress; }
    int totalCount() const { return mTotalCount; }

    Q_INVOKABLE void repair(DatabaseRepairWorker::RepairMode recordRepairMode,
                            DatabaseRepairWorker::RepairMode fileRepairMode);

signals:
    void errorCodeChanged(DatabaseRepairWorker::ErrorCode);
    void operationChanged(DatabaseRepairWorker::Operation);
    void progressChanged(int);
    void totalCountChanged(int);

private:
    void doRepairFiles();
    void doRepairRecords();

    void setErrorCode(ErrorCode errorCode)
    {
        if (errorCode != mErrorCode)
            emit errorCodeChanged(mErrorCode = errorCode);
    }

    void setOperation(Operation operation)
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

private slots:
    void doRepair();

private:
    QThread mThread;

    Operation mOperation = Operation::NotStarted;
    ErrorCode mErrorCode = ErrorCode::None;

    int mProgress = -1;
    int mTotalCount = -1;

    RepairMode mRecordRepairMode = RepairMode::Remove;
    RepairMode mFileRepairMode = RepairMode::Remove;
};

Q_DECLARE_METATYPE(DatabaseRepairWorker::ErrorCode)
Q_DECLARE_METATYPE(DatabaseRepairWorker::Operation)
Q_DECLARE_METATYPE(DatabaseRepairWorker::RepairMode)

#endif
