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
#include <QEventLoop>
#include <QTimer>
#include <QtQuick>

#include <FLAC/stream_decoder.h>

#include <libcallrecorder/callrecorderexception.h>
#include <libcallrecorder/database.h>
#include <libcallrecorder/eventstablemodel.h>
#include <libcallrecorder/phonenumberstablemodel.h>
#include <libcallrecorder/settings.h>
#include <libcallrecorder/sqlcursor.h>

using ErrorCode = DatabaseRepairHelper::ErrorCode;
using Operation = DatabaseRepairHelper::Operation;

class DatabaseRepairException : public CallRecorderException
{
public:
    explicit DatabaseRepairException(ErrorCode errorCode, QString what)
        : CallRecorderException(what),
          mErrorCode(errorCode)
    {
    }

    virtual ~DatabaseRepairException() throw()
    {
    }

    ErrorCode errorCode() const { return mErrorCode; }

private:
    ErrorCode mErrorCode = ErrorCode::None;
};

FLAC__StreamDecoderWriteStatus _flacWriteCallback(const FLAC__StreamDecoder*,
                                                  const FLAC__Frame*,
                                                  const FLAC__int32* const[],
                                                  void*)
{
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void _flacErrorCallback(const FLAC__StreamDecoder*,
                        FLAC__StreamDecoderErrorStatus errorStatus,
                        void*)
{
    qDebug() << errorStatus;
}

DatabaseRepairWorker::DatabaseRepairWorker(RepairMode recordRepairMode,
                                           RepairMode fileRepairMode,
                                           QObject* parent)
    : QObject(parent),
      QRunnable(),
      mRecordRepairMode(recordRepairMode),
      mFileRepairMode(fileRepairMode)
{
    qDebug();    
}

DatabaseRepairWorker::~DatabaseRepairWorker()
{
    qDebug();
}

void DatabaseRepairWorker::repairFiles()
{
    qDebug();

    emit operationChanged(Operation::ProcessingOrphanedFiles);
    emit totalCountChanged(-1);
    emit progressChanged(-1);

    QScopedPointer< Database > db(new Database());
    QScopedPointer< EventsTableModel > events(new EventsTableModel(db.data()));
    QScopedPointer< PhoneNumbersTableModel > phoneNumbers(new PhoneNumbersTableModel(db.data()));
    QScopedPointer< Settings > settings(new Settings());

    auto fiList = QDir(settings->outputLocation()).entryInfoList(
                QStringList() << QString("*.flac"), QDir::Files);

    int progress = 0;

    emit totalCountChanged(fiList.size());
    emit progressChanged(progress = 0);

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
                                ErrorCode::UnableToRemoveOrphanedFile, file.errorString());
                }
            }
            else if (mFileRepairMode == RepairMode::Restore)
            {
                qDebug() << "restoring orphaned file" << fi.fileName();

                // sample file name: 2016-06-20T17_11_34+02_00_+12345677890_out.flac

                QString baseName = fi.baseName();

                QString timeStamp = baseName.left(25).replace(QChar('_'), QChar(':'));
                QString eventType = baseName.mid(baseName.lastIndexOf(QChar('_')) + 1);

                QString phoneNumber = baseName.mid(
                            timeStamp.length() + 1,
                            baseName.length() - timeStamp.length() - eventType.length() - 2);

                qDebug() << "restored file parts" << timeStamp << eventType << phoneNumber;

                quint64 duration = 0;

                QByteArray fileName = fi.absoluteFilePath().toUtf8();

                auto* decoder = FLAC__stream_decoder_new();
                auto decoderStatus = FLAC__stream_decoder_init_file(
                            decoder,
                            fileName.constData(),
                            &_flacWriteCallback,
                            NULL,
                            &_flacErrorCallback,
                            NULL);

                if (decoderStatus == FLAC__STREAM_DECODER_INIT_STATUS_OK &&
                    FLAC__stream_decoder_process_until_end_of_metadata(decoder) &&
                    FLAC__stream_decoder_process_single(decoder))
                {
                    auto totalSamples = FLAC__stream_decoder_get_total_samples(decoder);
                    auto sampleRate = FLAC__stream_decoder_get_sample_rate(decoder);

                    qDebug() << "total samples" << totalSamples;
                    qDebug() << "sample rate" << sampleRate;

                    if (sampleRate > 0)
                        duration = totalSamples / sampleRate;
                }
                else
                {
                    qDebug() << "FLAC decoder init status" << decoderStatus;
                    qDebug() << "FLAC decoder state" << FLAC__stream_decoder_get_state(decoder);
                }

                FLAC__stream_decoder_finish(decoder);
                FLAC__stream_decoder_delete(decoder);

                auto id = events->add(QDateTime::fromString(timeStamp, Qt::ISODate),
                                      phoneNumbers->getIdByLineIdentification(phoneNumber),
                                      EventsTableModel::eventType(eventType),
                                      EventsTableModel::Done,
                                      duration,
                                      fi.fileName(),
                                      fi.size());

                if (id == -1)
                    throw DatabaseRepairException(ErrorCode::UnableToRestoreOrphanedFile,
                                                  QLatin1String("Error writing to database"));
            }
        }

        emit progressChanged(++progress);
    }
}

void DatabaseRepairWorker::repairRecords()
{
    qDebug();

    emit totalCountChanged(-1);
    emit progressChanged(-1);
    emit operationChanged(Operation::ProcessingOrphanedRecords);

    QScopedPointer< Database > db(new Database());
    QScopedPointer< Settings > settings(new Settings());
    QScopedPointer< SqlCursor > cursor(db->select("SELECT ID, FileName FROM Events"));

    if (cursor.isNull())
    {
        throw DatabaseRepairException(
                    ErrorCode::UnableToRetrieveOrphanedRecords, db->lastError());
    }

    int progress;

    emit totalCountChanged(cursor->size());
    emit progressChanged(progress = 0);

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

        emit progressChanged(++progress);
    }
}

void DatabaseRepairWorker::run()
{
    qDebug();

    emit started();

    ErrorCode errorCode = ErrorCode::None;

    try
    {
        if (mRecordRepairMode != RepairMode::Skip)
            repairRecords();

        if (mFileRepairMode != RepairMode::Skip)
            repairFiles();
    }
    catch (DatabaseRepairException& e)
    {
        qDebug() << e.qWhat();

        errorCode = e.errorCode();
    }
    catch (...)
    {
        errorCode = ErrorCode::UnhandledException;
    }

    emit operationChanged(Operation::Complete);
    emit totalCountChanged(1);
    emit progressChanged(1);
    emit finished(errorCode);
}

