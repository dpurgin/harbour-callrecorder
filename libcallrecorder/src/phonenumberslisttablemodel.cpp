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

    PhoneNumbersListTableModelPrivate(PhoneNumbersListTableModel* qptr,
                                      QString _tableName,
                                      Database* _db)
        : q(qptr),
          db(_db),
          tableName(_tableName),
          dirtyIndex(0),
          rowCount(0)
    {
        QString stmt("SELECT COUNT(ID) AS Total FROM " % tableName);

        QScopedPointer< SqlCursor > cursor(db->select(stmt));

        if (cursor->next())
            rowCount = cursor->value("Total").toInt();
    }

    // adds a row to dirty data
    // returns row id of a newly added data
    int add(int phoneNumberId)
    {
        qDebug() << phoneNumberId;

        // get phone number representation to show in ListView
        QString stmt("SELECT LineIdentification FROM PhoneNumbers WHERE ID = :phoneNumberId");

        Database::SqlParameters params;
        params.insert(":phoneNumberId", phoneNumberId);

        QScopedPointer< SqlCursor > cursor(db->select(stmt, params));

        QString lineIdentification;

        if (cursor->next())
            lineIdentification = cursor->value("LineIdentification").toString();

        // now prepare dirty row and insert to dirty data and
        // to model index <=> oid mapping
        Row dirtyRow;

        int oid = --dirtyIndex;

        dirtyRow.insert(PhoneNumbersListTableModel::ID, oid);
        dirtyRow.insert(PhoneNumbersListTableModel::PhoneNumberID, phoneNumberId);
        dirtyRow.insert(PhoneNumbersListTableModel::PhoneNumberIDRepresentation, lineIdentification);

        dirtyData.insert(oid, dirtyRow);

        // now add model index to oid mapping
        displayToOidMapping.insert(displayToOidMapping.size(), oid);

        rowCount++;

        emit q->rowCountChanged();

        return oid;
    }

    bool ensureRange(int displayIndex)
    {
        bool result = true;

        if (!displayToOidMapping.contains(displayIndex))
        {
            const int pageSize = 100;

            int offset = (displayIndex / pageSize) * pageSize;
            int limit = pageSize;

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
                        "\n    List.ID"
                        "\nLIMIT"
                        "\n    " % QString::number(limit) %
                        "\nOFFSET"
                        "\n    " % QString::number(offset));

            QScopedPointer< SqlCursor > cursor(db->select(stmt));

            if (cursor.isNull())
            {
                qDebug() << db->lastError();
                result = false;
            }
            else
            {
                for (int i = offset; cursor->next(); i++)
                {
                    int oid = cursor->value("ID").toInt();

                    Row row;

                    row.insert(PhoneNumbersListTableModel::ID,
                               oid);

                    row.insert(PhoneNumbersListTableModel::PhoneNumberID,
                               cursor->value("PhoneNumberID").toInt());

                    row.insert(PhoneNumbersListTableModel::PhoneNumberIDRepresentation,
                               cursor->value("PhoneNumberIDRepresentation"));

                    data.insert(oid, row);
                    displayToOidMapping.insert(i, oid);
                    oidToDisplayMapping.insert(oid, i);
                }
            }
        }

        return result;
    }

    // Persists all dirty data.
    // Row count is not changed.
    // TODO: check if some of dirty data was not written
    bool submit()
    {
        bool result = true;

        QString stmt(QLatin1String("INSERT INTO ") % tableName %
                     QLatin1String("(PhoneNumberID) VALUES(:phoneNumberId)"));

        // for each dirty row persist it in the database
        for (Rowset::const_iterator cit = dirtyData.cbegin();
             cit != dirtyData.cend();
             cit++)
        {
            Database::SqlParameters params;

            params.insert(":phoneNumberId",
                          cit->value(PhoneNumbersListTableModel::PhoneNumberID).toInt());

            int persistentOid = db->insert(stmt, params);

            // add persisted row into persistent data
            if (persistentOid != -1)
            {
                // copy row from dirty data and update its OID
                Row row(*cit);
                row.insert(PhoneNumbersListTableModel::ID, persistentOid);

                data.insert(persistentOid, row);

                // update mapping
                int dirtyOid = cit.key();
                int displayId = oidToDisplayMapping.value(dirtyOid);

                displayToOidMapping.insert(displayId, persistentOid);
                oidToDisplayMapping.insert(persistentOid, displayId);

                oidToDisplayMapping.remove(dirtyOid);
            }
        }

        // clear dirty data
        dirtyData.clear();
        dirtyIndex = 0;

        return result;
    }

    PhoneNumbersListTableModel* q;

    // table contents
    Rowset data;

    // model index => OID mapping
    QHash< int, int > displayToOidMapping;

    // OID => model index mapping
    QHash< int, int > oidToDisplayMapping;

    // non-persistent data
    // OIDs are generated virtually starting from -1 to -inf
    Rowset dirtyData;
    int dirtyIndex;

    int rowCount;

    Database* db;    
    QString tableName;   
};

PhoneNumbersListTableModel::PhoneNumbersListTableModel(
        QString tableName,
        Database* db,
        QObject* parent)
    : QAbstractListModel(parent),
      d(new PhoneNumbersListTableModelPrivate(this, tableName, db))
{
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

    int id = d->add(phoneNumberId);

    emit endInsertRows();

    return (id != -1);
}

QVariant PhoneNumbersListTableModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if (d->ensureRange(index.row()))
    {
        // index.row() references to a display position
        // OIDs can be non-continuous (e.g., a user deleted one of the earlier entries in table),
        // so we use model index to OID mapping

        if (d->displayToOidMapping.contains(index.row()))
        {
            int oid = d->displayToOidMapping.value(index.row());

            // if oid is < 0, this is dirty data.
            // persistent data otherwise
            PhoneNumbersListTableModelPrivate::Row record = (oid < 0?
                                                                 d->dirtyData.value(oid):
                                                                 d->data.value(oid));

            result = record.value(role, QVariant());
        }
    }

    return result;
}

QHash< int, QByteArray > PhoneNumbersListTableModel::roleNames() const
{
    static QHash< int, QByteArray > names;

    if (names.isEmpty())
    {
        names.insert(ID, "ID");
        names.insert(PhoneNumberID, "PhoneNumberID");
        names.insert(PhoneNumberIDRepresentation, "PhoneNumberIDRepresentation");
    }

    return names;
}

int PhoneNumbersListTableModel::rowCount(const QModelIndex&) const
{
    return d->rowCount;
}

bool PhoneNumbersListTableModel::submitAll()
{
    return d->submit();
}

QString PhoneNumbersListTableModel::tableName() const
{
    return d->tableName;
}
