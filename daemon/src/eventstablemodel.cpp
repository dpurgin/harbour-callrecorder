#include "eventstablemodel.h"

#include "application.h"
#include "database.h"

EventsTableModel::EventsTableModel()
{
    qDebug() << __PRETTY_FUNCTION__;
}

EventsTableModel::~EventsTableModel()
{
    qDebug() << __PRETTY_FUNCTION__;
}

int EventsTableModel::add(const QDateTime& timeStamp, EventType eventType)
{
    qDebug() << __PRETTY_FUNCTION__;

    static QString insertStatement("INSERT INTO Events(TimeStamp, EventTypeID) VALUES(:timeStamp, :eventTypeId);");

    Database::SqlParameters params;
    params.insert(QLatin1String("timeStamp"), timeStamp);
    params.insert(QLatin1String("eventTypeId"), static_cast< int >(eventType));

    // TODO: process insert errors
    return app->database()->insert(insertStatement, params);
}

