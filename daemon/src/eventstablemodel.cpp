/*
    Call Recorder for SailfishOS
    Copyright (C) 2014  Dmitriy Purgin <dpurgin@gmail.com>

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

#include "eventstablemodel.h"

#include <QDebug>
#include <QStringBuilder>
#include <QStringList>

#include "application.h"

#include <libcallrecorder/database.h>

class EventsTableModel::EventsTableModelPrivate
{
    friend class EventsTableModel;
};

EventsTableModel::EventsTableModel()
    : d(new EventsTableModelPrivate())
{
    qDebug();
}

EventsTableModel::~EventsTableModel()
{
    qDebug();
}

int EventsTableModel::add(const QDateTime& timeStamp, int phoneNumberId, EventType eventType, RecordingState recordingState)
{
    qDebug() << timeStamp << phoneNumberId << eventType;

    static QString insertStatement(
                "\nINSERT INTO Events"
                "\n("
                "\n    TimeStamp,"
                "\n    PhoneNumberID,"
                "\n    EventTypeID,"
                "\n    RecordingStateID"
                "\n)"
                "\nVALUES"
                "\n("
                "\n    :timeStamp,"
                "\n    :phoneNumberId,"
                "\n    :eventTypeId,"
                "\n    :recordingStateId"
                "\n);");

    Database::SqlParameters params;
    params.insert(QLatin1String(":timeStamp"), Application::getIsoTimeStamp(timeStamp));
    params.insert(QLatin1String(":phoneNumberId"), phoneNumberId);
    params.insert(QLatin1String(":eventTypeId"), static_cast< int >(eventType));
    params.insert(QLatin1String(":recordingStateId"), static_cast< int >(recordingState));

    // TODO: process insert errors
    return daemon->database()->insert(insertStatement, params);
}

void EventsTableModel::remove(int id)
{
    qDebug() << id;

    static QString statement("DELETE FROM Events WHERE ID = :id;");

    Database::SqlParameters params;
    params.insert(QLatin1String(":id"), id);

    // TODO: process errors (maybe?)
    daemon->database()->execute(statement, params);
}

void EventsTableModel::update(int id, const QVariantMap& items)
{
    qDebug() << id << items;

    static QString statement("UPDATE Events SET %1 WHERE ID = :id");

    QStringList setList;

    Database::SqlParameters params;

    params.insert(":id", id);

    for (QVariantMap::const_iterator cit = items.cbegin();
         cit != items.cend();
         cit++)
    {
        params.insert(QLatin1Char(':') % cit.key(), cit.value());
        setList << QString(cit.key() % QLatin1String(" = :") % cit.key());
    }

    // TODO: process errors
    daemon->database()->execute(statement.arg(setList.join(QChar(','))), params);
}
