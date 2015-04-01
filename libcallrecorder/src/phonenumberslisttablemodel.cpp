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

    // single database row
    // key - column index
    // value - column value
    typedef QHash< int, QVariant > Row;

    // database rowset
    // key - OID (value of ID column)
    // value - row with OID
    typedef QHash< int, Row > Rowset;

    // Fetches single row from database.
    // Used after inserting a new record to update cached data.
    void fetch(int id)
    {
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
            "\n        PhoneNumbers.ID = List.PhoneNumberID"
            "\nWHERE"
            "\n    List.ID = :id");

        Database::SqlParameters params;
        params.insert(":id", id);

        QScopedPointer< SqlCursor > cursor(db->select(stmt, params));

        if (cursor->next())
        {
            Row record;

            QStringList columns = cursor->columns();

            for (int c = 0; c < columns.size(); c++)
                record.insert(c, cursor->value(columns[c]));

            int oid = record.value(0).toInt();

            // If this record is new, a mapping to display position at the end of the list is added.
            // Otherwise, such a mapping already exists.
            if (!data.contains(oid))
                displayToOidMapping.insert(displayToOidMapping.size(), oid);

            data.insert(record.value(0).toInt(), record);

            rowCount = data.size();
        }
    }

    // Prefetches all data from a table.
    // This means that we can safely assume that the order of model indices will be consistent
    // for now
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
            "\n        PhoneNumbers.ID = List.PhoneNumberID"
            "\nORDER BY"
            "\n    List.ID");

        QScopedPointer< SqlCursor > cursor(db->select(stmt));

        for (int i = 0; cursor->next(); i++)
        {
            Row record;

            QStringList columns = cursor->columns();

            for (int c = 0; c < columns.size(); c++)
                record.insert(c, cursor->value(columns[c]));

            // get record OID and add to data
            data.insert(record.value(0).toInt(), record);

            // map this record to display position
            displayToOidMapping.insert(i, record.value(0).toInt());
        }

        rowCount = data.size();
    }

    // table contents
    Rowset data;

    // model index => OID mapping
    QHash< int, int > displayToOidMapping;

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

    emit beginInsertRows(QModelIndex(), rowCount(), rowCount());

    QString stmt("INSERT INTO " % tableName() % "(PhoneNumberID) VALUES(:phoneNumberId)");

    Database::SqlParameters params;
    params.insert(":phoneNumberId", phoneNumberId);

    int id = d->db->insert(stmt, params);

    d->fetch(id);

    emit endInsertRows();

    return (id != -1);
}

QVariant PhoneNumbersListTableModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    // index.row() references to a display position
    // OIDs can be non-continuous (e.g., a user deleted one of the earlier entries in table),
    // so we use model index to OID mapping

    if (d->displayToOidMapping.contains(index.row()))
    {
        PhoneNumbersListTableModelPrivate::Row record =
                d->data.value(d->displayToOidMapping.value(index.row()));

        result = record.value(role - Qt::UserRole, QVariant());
    }

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
