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

#include "databaserepairhelper.h"

#include <QThreadPool>
#include <QtQml>

#include "databaserepairworker.h"

namespace
{
    void initQml()
    {
        qmlRegisterType< DatabaseRepairHelper >(
                    "kz.dpurgin.callrecorder.DatabaseRepairHelper", 1, 0, "DatabaseRepairHelper");

        qRegisterMetaType< DatabaseRepairHelper::ErrorCode >();
        qRegisterMetaType< DatabaseRepairHelper::Operation >();
        qRegisterMetaType< DatabaseRepairHelper::RepairMode >();
    }

    Q_CONSTRUCTOR_FUNCTION(initQml)
}

DatabaseRepairHelper::DatabaseRepairHelper(QObject* parent)
    : QObject(parent)
{
    qDebug();
}

DatabaseRepairHelper::~DatabaseRepairHelper()
{
    qDebug();
}

void DatabaseRepairHelper::repair(RepairMode recordRepairMode, RepairMode fileRepairMode)
{
    setOperation(Operation::Starting);
    setErrorCode(ErrorCode::None);

    QScopedPointer< DatabaseRepairWorker > worker(
                new DatabaseRepairWorker(recordRepairMode, fileRepairMode));

    connect(worker.data(), &DatabaseRepairWorker::finished,
            [this](ErrorCode errorCode) { setErrorCode(errorCode); });

    connect(worker.data(), &DatabaseRepairWorker::operationChanged,
            this, &DatabaseRepairHelper::setOperation);
    connect(worker.data(), &DatabaseRepairWorker::progressChanged,
            this, &DatabaseRepairHelper::setProgress);
    connect(worker.data(), &DatabaseRepairWorker::totalCountChanged,
            this, &DatabaseRepairHelper::setTotalCount);

    if (QThreadPool::globalInstance()->tryStart(worker.data()))
        worker.take();
    else
    {
        setOperation(Operation::Complete);
        setErrorCode(ErrorCode::UnableToStart);
    }
}


