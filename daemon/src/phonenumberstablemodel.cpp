#include "phonenumberstablemodel.h"

#include <QDebug>

#include "application.h"
#include "callrecorderexception.h"
#include "database.h"
#include "sqlcursor.h"

class PhoneNumbersTableModel::PhoneNumbersTableModelPrivate
{
    friend class PhoneNumbersTableModel;
};

PhoneNumbersTableModel::PhoneNumbersTableModel()
{
    qDebug() << __PRETTY_FUNCTION__;
}

PhoneNumbersTableModel::~PhoneNumbersTableModel()
{
    qDebug() << __PRETTY_FUNCTION__;
}

int PhoneNumbersTableModel::getIdByLineIdentification(const QString& lineIdentification)
{
    qDebug() << __PRETTY_FUNCTION__;

    int id = -1;

    static QString selectStatement("SELECT ID FROM PhoneNumbers WHERE LineIdentification = :lineIdentification;");

    Database::SqlParameters params;
    params.insert(QLatin1String("lineIdentification"), lineIdentification);

    QScopedPointer< SqlCursor > cursor(app->database()->select(selectStatement, params));

    // no data found - insert new row into PhoneNumbers
    // TODO: process errors when selecting from DB
    if (cursor.isNull())
    {
        static QString insertStatement("INSERT INTO PhoneNumbers(LineIdentification) VALUES(:lineIdentification);");

        // TODO: process errors when inserting to DB
        id = app->database()->insert(insertStatement, params);
    }
    // phone number was already contacted
    else if (cursor->next())
    {
        id = cursor->value("ID").toInt();
    }
    else
    {
        qCritical() << __PRETTY_FUNCTION__ << ": unable to retrieve PhoneNumbers.ID for " << lineIdentification;
    }

    return id;
}
