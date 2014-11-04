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
#include <QFile>
#include <QHash>
#include <QSqlError>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QVector>

#include "database.h"
#include "sqlcursor.h"

class EventsTableModel::EventsTableModelPrivate
{
    friend class EventsTableModel;

    EventsTableModelPrivate()
        : db(NULL),
          rowCount(0),
          pageSize(100)
    {
    }

private:
    void guaranteeRange(int rowIndex)
    {
        if (!dataByRowIndex.contains(rowIndex))
        {
            int pageStart = (rowIndex / pageSize) * pageSize;

            qDebug() << "Fetching " << pageSize << " items starting from " << pageStart;

            static QString stmt(
                        "\nSELECT"
                        "\n    Events.*,"
                        "\n    PhoneNumbers.LineIdentification AS PhoneNumberIDRepresentation"
                        "\nFROM"
                        "\n    Events"
                        ""
                        "\n    LEFT JOIN"
                        "\n        PhoneNumbers"
                        "\n    ON"
                        "\n        PhoneNumbers.ID = Events.PhoneNumberID"
                        "\nORDER BY"
                        "\n    Events.TimeStamp DESC"
                        "\nLIMIT :pageSize"
                        "\nOFFSET :pageStart");

            Database::SqlParameters params;
            params.insert(":pageSize", pageSize);
            params.insert(":pageStart", pageStart);

            QScopedPointer< SqlCursor > cursor(db->select(stmt, params));

            QStringList columns = cursor->columns();

            for (int i = 0; cursor->next(); i++)
            {
                QHash< QString, QVariant > record;

                for (int j = 0; j < columns.size(); j++)
                    record.insert(columns[j], cursor->value(columns[j]));

                dataByRowIndex.insert(pageStart + i, record);
                dataByOID.insert(cursor->value("ID").toInt(), record);
            }
        }
    }

private:
    Database* db;

    QHash< int, QHash< QString, QVariant > > dataByRowIndex;
    QHash< int, QHash< QString, QVariant > > dataByOID;

    int pageSize;

    int rowCount;
};

EventsTableModel::EventsTableModel(Database* db, QObject* parent)
    : QAbstractListModel(parent),
      d(new EventsTableModelPrivate())
{
    qDebug() << __PRETTY_FUNCTION__;

    d->db = db;

    QScopedPointer< SqlCursor > cursor(d->db->select("SELECT COUNT(ID) AS RowCount FROM Events"));

    while (cursor->next())
    {
        d->rowCount = cursor->value("RowCount").toInt();

        qDebug() << "Retrieved row count: " << d->rowCount;
    }

}

EventsTableModel::~EventsTableModel()
{
    qDebug() << __PRETTY_FUNCTION__;

    delete d;
}

QVariant EventsTableModel::data(const QModelIndex& item, int role) const
{    
    d->guaranteeRange(item.row());

    QString fieldName(roleNames().value(role));

    QHash< QString, QVariant > record = d->dataByRowIndex.value(item.row());

    QVariant result = record.value(fieldName);

    return result;
}

bool EventsTableModel::removeRow(int rowIndex, const QModelIndex& parent)
{
    return removeRows(rowIndex, 1, parent);
}

bool EventsTableModel::removeRows(int rowIndex, int count, const QModelIndex& parent)
{
    qDebug() << "rowIndex: " << rowIndex << ", count: " << count;

    int failures = 0;

    beginRemoveRows(parent, rowIndex, rowIndex + count);

    for (int i = rowIndex; i < rowIndex + count; i++)
    {
        d->guaranteeRange(i);

        QHash< QString, QVariant > record = d->dataByRowIndex.value(i);

        Database::SqlParameters params;
        params.insert(":id", record.value("ID"));

        if (d->db->execute("DELETE FROM Events WHERE ID = :id", params))
        {
            QString location = QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
                    QLatin1String("/data/") %
                    record.value("FileName").toString();

            qDebug() << "removing" << location;

            QFile(location).remove();
        }
        else
        {
            failures++;

            qDebug() << "error removing item: " << d->db->lastError();
        }

    }

    if (failures != count)
    {
        d->rowCount -= count + failures;
        emit rowCountChanged();
    }

    endRemoveRows();

    return (failures != count);
}

//bool EventsTableModel::removeItem(const QString& id, const QString& fileName)
//{
//    qDebug() << "id: " << id << ", fileName: " << fileName;

//    Database::SqlParameters params;
//    params.insert(":id", id);

//    if (d->db->execute("DELETE FROM Events WHERE ID = :id", params))
//    {
//        QString location = QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
//                QLatin1String("/data/") %
//                fileName;

//        qDebug() << "removing" << location;

//        QFile(location).remove();

//        select();
//    }
//    else
//        qDebug() << "error removing item: " << d->db->lastError();

//    return true;
//}

QHash< int, QByteArray > EventsTableModel::roleNames() const
{
    static QHash< int, QByteArray > roles;

    if (roles.empty())
    {
        QStringList tableColumns = d->db->tableColumns("Events");

        foreach (QString columnName, tableColumns)
            roles.insert(Qt::UserRole + roles.size(), columnName.toUtf8());

        roles.insert(Qt::UserRole + roles.size(), "PhoneNumberIDRepresentation");
    }

    return roles;
}

int EventsTableModel::rowCount(const QModelIndex&) const
{
    return d->rowCount;
}
