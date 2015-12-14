/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2015 Dmitriy Purgin <dpurgin@gmail.com>

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
#include <QSqlError>
#include <QSqlRecord>

#include "database.h"
#include "sqlcursor.h"

class PhoneNumbersTableModel::PhoneNumbersTableModelPrivate
{
    friend class PhoneNumbersTableModel;

    Database* db;
};

PhoneNumbersTableModel::PhoneNumbersTableModel(Database* db, QObject *parent)
    : QAbstractListModel(parent),
      d(new PhoneNumbersTableModelPrivate)
{
    d->db = db;
}

PhoneNumbersTableModel::~PhoneNumbersTableModel()
{
    delete d;
}

bool PhoneNumbersTableModel::contains(const QString& lineIdentification)
{
    static QString stmt(
                "SELECT ID FROM PhoneNumbers WHERE LineIdentification = :lineIdentification");

    Database::SqlParameters params;
    params.insert(QLatin1String(":lineIdentification"), lineIdentification);

    QScopedPointer< SqlCursor > cursor(d->db->select(stmt, params));

    return cursor->next();
}

QVariant PhoneNumbersTableModel::data(const QModelIndex& index, int role) const
{
    return QVariant();
}

int PhoneNumbersTableModel::getIdByLineIdentification(const QString& lineIdentification)
{
    qDebug();

    int id = -1;

    static QString selectStatement(
                "SELECT ID FROM PhoneNumbers WHERE LineIdentification = :lineIdentification");

    Database::SqlParameters params;
    params.insert(QLatin1String(":lineIdentification"), lineIdentification);

    QScopedPointer< SqlCursor > cursor(d->db->select(selectStatement, params));

    // TODO: process errors when selecting from DB
    if (cursor.isNull())
    {
        qCritical() <<  QLatin1String(": unable to retrieve PhoneNumbers.ID for ") <<
                        lineIdentification;
    }
    // phone number was already contacted
    else if (cursor->next())
    {
        id = cursor->value(QLatin1String("ID")).toInt();
    }
    // no data found - insert new row into PhoneNumbers and return its id
    else
    {
        static QString insertStatement(
                    "INSERT INTO PhoneNumbers(LineIdentification) VALUES(:lineIdentification)");

        Database::SqlParameters params;
        params.insert(":lineIdentification", lineIdentification);

        id = d->db->insert(insertStatement, params);
    }

    return id;
}

int PhoneNumbersTableModel::rowCount(const QModelIndex&) const
{
    return 0;
}
