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

#include "phonenumberslisttablemodel.h"

#include <QDebug>
#include <QHash>
#include <QStringBuilder>

#include "database.h"
#include "sqlcursor.h"

class PhoneNumbersListTableModel::PhoneNumbersListTableModelPrivate
{
    friend class PhoneNumbersListTableModel;

    void prefetch()
    {
        data.clear();

        QString stmt(
            "\nSELECT"
            "\n    List.ID,"
            "\n    List.PhoneNumberID,"
            "\n    PhoneNumbers.LineIdentification AS PhoneNumberIDRepresentation"
            "\nFROM"
            "\n    " % tableName % " AS List"
            "\n    LEFT JOIN"
            "\n        PhoneNumbers"
            "\n    ON"
            "\n        PhoneNumbers.ID = List.PhoneNumberID");

        QScopedPointer< SqlCursor > cursor(db->select(stmt));


        for (int i = 0; cursor->next(); i++)
        {
            QHash< int, QVariant > record;

            QStringList columns = cursor->columns();

            for (int c = 0; c < columns.size(); c++)
                record.insert(c + Qt::UserRole, cursor->value(columns[c]));

            data.insert(i, record);
        }

        rowCount = data.size();
    }

    QHash< int, QHash< int, QVariant > > data;
    int rowCount;

    Database* db;    
    QString tableName;   
};

PhoneNumbersListTableModel::PhoneNumbersListTableModel(
        QString tableName, Database* db, QObject *parent)
    : QAbstractListModel(parent),
      d(new PhoneNumbersListTableModelPrivate())
{
    d->db = db;
    d->tableName = tableName;
    d->rowCount = -1;

    d->prefetch();
}

PhoneNumbersListTableModel::~PhoneNumbersListTableModel()
{
    delete d;
}

bool PhoneNumbersListTableModel::contains(const QString &lineIdentification) const
{
    bool result = false;

    QString stmt(
            "\nSELECT"
            "\n    List.ID"
            "\nFROM"
            "\n    " % tableName() % " AS List"
            "\n"
            "\n    LEFT JOIN"
            "\n        PhoneNumbers"
            "\n    ON"
            "\n        PhoneNumbers.ID = List.PhoneNumberID"
            "\nWHERE"
            "\n    PhoneNumbers.LineIdentification = :lineIdentification");

    Database::SqlParameters params;
    params.insert(":lineIdentification", lineIdentification);

    SqlCursor* cursor = d->db->select(stmt, params);

    if (cursor)
        result = cursor->next();
    else
        qCritical() << "Unable to execute query: " << d->db->lastError();

    return result;
}

bool PhoneNumbersListTableModel::add(int phoneNumberId)
{
    qDebug();

    QString stmt("INSERT INTO " % tableName() % "(PhoneNumberID) VALUES(:phoneNumberId)");

    Database::SqlParameters params;
    params.insert(":phoneNumberId", phoneNumberId);

    int id = d->db->insert(stmt, params);

    return (id != -1);
}

QVariant PhoneNumbersListTableModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if (d->data.contains(index.row()))
        result = d->data.value(index.row()).value(role, QVariant());

    return result;
}

QHash< int, QByteArray > PhoneNumbersListTableModel::roleNames() const
{
    static QHash< int, QByteArray > names;

    if (names.isEmpty())
    {
        names.insert(Qt::UserRole, "ID");
        names.insert(Qt::UserRole + 1, "PhoneNumberID");
        names.insert(Qt::UserRole + 2, "PhoneNumberIDRepresentation");
    }

    return names;
}

int PhoneNumbersListTableModel::rowCount(const QModelIndex&) const
{
    return d->rowCount;
}

QString PhoneNumbersListTableModel::tableName() const
{
    return d->tableName;
}
