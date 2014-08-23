#include "eventstablemodel.h"

#include <QDebug>
#include <QStringBuilder>
#include <QStringList>

#include "application.h"
#include "database.h"

class EventsTableModel::EventsTableModelPrivate
{
    friend class EventsTableModel;
};

EventsTableModel::EventsTableModel()
    : d(new EventsTableModelPrivate())
{
    qDebug() << __PRETTY_FUNCTION__;
}

EventsTableModel::~EventsTableModel()
{
    qDebug() << __PRETTY_FUNCTION__;
}

int EventsTableModel::add(const QDateTime& timeStamp, int phoneNumberId, EventType eventType, RecordingState recordingState)
{
    qDebug() << __PRETTY_FUNCTION__ << timeStamp << phoneNumberId << eventType;

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
    params.insert(QLatin1String(":timeStamp"), timeStamp);
    params.insert(QLatin1String(":phoneNumberId"), phoneNumberId);
    params.insert(QLatin1String(":eventTypeId"), static_cast< int >(eventType));
    params.insert(QLatin1String(":recordingStateId"), static_cast< int >(recordingState));

    // TODO: process insert errors
    return app->database()->insert(insertStatement, params);
}

void EventsTableModel::remove(int id)
{
    qDebug() << __PRETTY_FUNCTION__ << id;

    static QString statement("DELETE FROM Events WHERE ID = :id;");

    Database::SqlParameters params;
    params.insert(QLatin1String(":id"), id);

    // TODO: process errors (maybe?)
    app->database()->execute(statement, params);
}

void EventsTableModel::update(int id, const QVariantMap& items)
{
    qDebug() << __PRETTY_FUNCTION__ << id << items;

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
    app->database()->execute(statement.arg(setList.join(QChar(','))), params);
}
