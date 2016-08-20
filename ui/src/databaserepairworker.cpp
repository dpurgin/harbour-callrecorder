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

#include "databaserepairworker.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QElapsedTimer>
#include <QtQuick>

#include <libcallrecorder/callrecorderexception.h>
#include <libcallrecorder/database.h>
#include <libcallrecorder/eventstablemodel.h>
#include <libcallrecorder/settings.h>
#include <libcallrecorder/sqlcursor.h>

namespace
{
    void initQ()
    {
        qmlRegisterType< DatabaseRepairWorker >(
                    "kz.dpurgin.callrecorder.DatabaseRepairWorker", 1, 0, "DatabaseRepairWorker");

        qRegisterMetaType< DatabaseRepairWorker::ErrorCode >();
        qRegisterMetaType< DatabaseRepairWorker::Operation >();
        qRegisterMetaType< DatabaseRepairWorker::RepairMode >();
    }

    Q_CONSTRUCTOR_FUNCTION(initQ)
}

class DatabaseRepairException : public CallRecorderException
{
public:
    explicit DatabaseRepairException(DatabaseRepairWorker::ErrorCode errorCode,
                                     QString what)
        : CallRecorderException(what),
          mErrorCode(errorCode)
    {
    }

    virtual ~DatabaseRepairException() throw()
    {
    }

    DatabaseRepairWorker::ErrorCode errorCode() const { return mErrorCode; }

private:
    DatabaseRepairWorker::ErrorCode mErrorCode;
};

DatabaseRepairWorker::DatabaseRepairWorker(QObject *parent)
    : QObject(parent)
{
    qDebug();

    moveToThread(&mThread);

    connect(&mThread, &QThread::started,
            this, &DatabaseRepairWorker::doRepair);
}

DatabaseRepairWorker::~DatabaseRepairWorker()
{
    qDebug();

    if (mThread.isRunning())
    {
        mThread.quit();
        mThread.wait();
    }
}

void DatabaseRepairWorker::doRepair()
{
    qDebug();

    setErrorCode(ErrorCode::None);

    try
    {
        if (mRecordRepairMode != RepairMode::Skip)
            doRepairRecords();

        if (mFileRepairMode != RepairMode::Skip)
            doRepairFiles();
    }
    catch (DatabaseRepairException& e)
    {
        qDebug() << e.qWhat();

        setErrorCode(e.errorCode());
    }
    catch (...)
    {
        setErrorCode(ErrorCode::UnhandledException);
    }

    setOperation(Operation::Complete);

    mThread.quit();
}

void DatabaseRepairWorker::doRepairFiles()
{
    qDebug();

    setOperation(Operation::ProcessingOrphanedFiles);
    setTotalCount(-1);
    setProgress(-1);

    QScopedPointer< Database > db(new Database());
    QScopedPointer< EventsTableModel > events(new EventsTableModel(db.data()));
    QScopedPointer< Settings > settings(new Settings());

    auto fiList = QDir(settings->outputLocation()).entryInfoList(
                QStringList() << QString("*.flac"), QDir::Files);

    setTotalCount(fiList.size());
    setProgress(0);

    foreach (QFileInfo fi, fiList)
    {
        EventsTableModel::Filters filters;
        filters.insert(EventsTableModel::FileName, fi.fileName());

        events->filter(filters);

        if (events->rowCount() == 0)
        {
            if (mFileRepairMode == RepairMode::Remove)
            {
                qDebug() << "removing orphaned file" << fi.fileName();

                QFile file(fi.absoluteFilePath());

                if (!file.remove())
                {
                    throw DatabaseRepairException(
                                ErrorCode::UnabletoRemoveOrphanedFile, file.errorString());
                }
            }
            else if (mFileRepairMode == RepairMode::Restore)
            {
                qDebug() << "restoring orphaned file" << fi.fileName();

                // sample file name: 2016-06-20T17_11_34+02_00_+12345677890_out.flac

                QString baseName = fi.baseName();

                QString timeStamp = baseName.left(25);
                QString eventType = fi.baseName().right(fi.baseName().lastIndexOf(QChar('_')) + 1);
                QString phoneNumber = baseName.mid(timeStamp.length(),
                                                   baseName.length() - eventType.length() - 1);

                qDebug() << "restored file parts" << timeStamp << eventType << phoneNumber;
            }
        }

        setProgress(progress() + 1);
    }
}

void DatabaseRepairWorker::doRepairRecords()
{
    qDebug();

    setTotalCount(-1);
    setProgress(-1);
    setOperation(Operation::ProcessingOrphanedRecords);

    QScopedPointer< Database > db(new Database());
    QScopedPointer< Settings > settings(new Settings());
    QScopedPointer< SqlCursor > cursor(db->select("SELECT ID, FileName FROM Events"));

    if (cursor.isNull())
    {
        throw DatabaseRepairException(
                    ErrorCode::UnableToRetrieveOrphanedRecords, db->lastError());
    }

    setTotalCount(cursor->size());
    setProgress(0);

    QDir outputLocationDir(settings->outputLocation());

    while (cursor->next())
    {
        QString fileName = cursor->value("FileName").toString();

        if (fileName.isEmpty() || !QFile(outputLocationDir.filePath(fileName)).exists())
        {
            if (mRecordRepairMode == RepairMode::Remove)
            {
                qDebug() << "removing orphaned record" << fileName;

                Database::SqlParameters params;
                params.insert(":id", cursor->value("ID").toInt());

                static QString deleteStmt("DELETE FROM Events WHERE ID = :id");

                if (!db->execute(deleteStmt, params))
                {
                    throw DatabaseRepairException(
                                ErrorCode::UnableToRemoveOrphanedRecord, db->lastError());
                }
            }
        }

        setProgress(progress() + 1);
    }
}

void DatabaseRepairWorker::repair(DatabaseRepairWorker::RepairMode recordRepairMode,
                                  DatabaseRepairWorker::RepairMode fileRepairMode)
{
    qDebug();

    if (!mThread.isRunning())
    {
        setErrorCode(ErrorCode::None);
        setOperation(Operation::Starting);

        mRecordRepairMode = recordRepairMode;
        mFileRepairMode = fileRepairMode;

        mThread.start();

        QElapsedTimer timer;
        timer.start();

        while (!mThread.isRunning() && !timer.hasExpired(30000))
            QThread::msleep(1);

        if (!mThread.isRunning())
        {
            setErrorCode(ErrorCode::UnableToStart);
            setOperation(Operation::Complete);
        }
    }
}
