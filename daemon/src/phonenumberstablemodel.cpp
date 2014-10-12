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

#include "phonenumberstablemodel.h"

#include <QDebug>

#include <libcallrecorder/callrecorderexception.h>
#include <libcallrecorder/database.h>
#include <libcallrecorder/sqlcursor.h>

#include "application.h"

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
    params.insert(QLatin1String(":lineIdentification"), lineIdentification);

    QScopedPointer< SqlCursor > cursor(app->database()->select(selectStatement, params));

    // TODO: process errors when selecting from DB
    if (cursor.isNull())
    {
        qCritical() << __PRETTY_FUNCTION__ << ": unable to retrieve PhoneNumbers.ID for " << lineIdentification;
    }
    // phone number was already contacted
    else if (cursor->next())
    {
        id = cursor->value("ID").toInt();
    }
    // no data found - insert new row into PhoneNumbers
    else
    {
        static QString insertStatement("INSERT INTO PhoneNumbers(LineIdentification) VALUES(:lineIdentification);");

        // TODO: process errors when inserting to DB
        id = app->database()->insert(insertStatement, params);
    }

    return id;
}
