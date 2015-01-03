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
#include <QDir>
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
          pageSize(100),
          rowCount(0)
    {
    }

private:
    void clearCache()
    {
        dataByRowIndex.clear();
        oidToRowIndex.clear();

        rowCount = 0;

        QScopedPointer< SqlCursor > cursor(db->select("SELECT COUNT(ID) AS RowCount FROM Events"));

        while (cursor->next())
        {
            rowCount = cursor->value("RowCount").toInt();

            qDebug() << "Retrieved row count: " << rowCount;
        }
    }

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
                oidToRowIndex.insert(record.value("ID").toInt(), pageStart + i);
            }
        }
    }

    void invalidatePage(int pageEntry)
    {
        int pageStart = (pageEntry / pageSize) * pageSize;

        qDebug() << "Invalidating page starting from " << pageStart;

        for (int i = pageStart; i < pageStart + pageSize; i++)
        {
            if (dataByRowIndex.contains(i))
            {
                oidToRowIndex.remove(dataByRowIndex.value(i).value("ID").toInt());

                dataByRowIndex.remove(i);
            }
        }
    }

    bool removeOid(int oid)
    {
        qDebug() << "oid: " << oid;

        bool result = true;

        Database::SqlParameters params;
        params.insert(":id", oid);

        QString fileName;

        if (oidToRowIndex.contains(oid))
        {
            fileName = dataByRowIndex.value(oidToRowIndex.value(oid)).value("FileName").toString();

            qDebug() << "found in cache: " << fileName;
        }
        else
        {
            QScopedPointer< SqlCursor > cursor(db->select("SELECT FileName FROM Events WHERE ID = :id", params));

            while (cursor->next())
                fileName = cursor->value("FileName").toString();

            qDebug() << "fetched from database: " << fileName;
        }

        if (db->execute("DELETE FROM Events WHERE ID = :id", params))
        {
            QString location = QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
                    QLatin1String("/data/") %
                    fileName;

            qDebug() << "Removing" << location;

            QFile file(location);

            if (!file.remove())
            {
                qWarning() << "Unable to remove file " << location << ": " << file.errorString();

                // result = false is not set since database row was deleted and there's need to emit
                // rowCountChanged
            }
        }
        else
        {
            result = false;

            qDebug() << "Error removing databse row: " << db->lastError();
        }

        return result;
    }

private:
    Database* db;

    QHash< int, QHash< QString, QVariant > > dataByRowIndex;
    QHash< int, int > oidToRowIndex;

    int pageSize;

    int rowCount;
};

EventsTableModel::EventsTableModel(Database* db, QObject* parent)
    : QAbstractListModel(parent),
      d(new EventsTableModelPrivate())
{
    qDebug() << __PRETTY_FUNCTION__;

    d->db = db;

    d->clearCache();
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

void EventsTableModel::refresh()
{
    emit beginResetModel();

    d->clearCache();    

    emit endResetModel();

    emit rowCountChanged();
}

bool EventsTableModel::removeAll()
{
    bool result = true;

    qDebug() << "removing all";

    if (d->db->execute("DELETE FROM Events"))
    {
        QString location = QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
                QLatin1String("/data");

        QDir dir(location);

        QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

        foreach (QString file, files)
        {
            qDebug() << "removing" << file;
            QFile(file).remove();
        }

        refresh();
    }
    else
        result = false;

    return result;
}

bool EventsTableModel::removeOids(const QList< int >& oids)
{
    qDebug() << oids;

    int failures = 0;

    QList< int > rowIndices;

    foreach (int oid, oids)
    {
        if (d->oidToRowIndex.contains(oid))
            rowIndices.push_back(d->oidToRowIndex.value(oid));
    }

    qSort(rowIndices.begin(), rowIndices.end(), qGreater< int >());

    foreach (int rowIndex, rowIndices)
    {
        beginRemoveRows(QModelIndex(), rowIndex, rowIndex);

        if (!d->removeOid(d->dataByRowIndex.value(rowIndex).value("ID").toInt()))
            failures++;

        endRemoveRows();
    }

    foreach (int oid, oids)
    {
        if (!d->oidToRowIndex.contains(oid))
            if (!d->removeOid(oid))
                failures++;
    }

    d->clearCache();

    if (failures != oids.size())
        emit rowCountChanged();

    return (failures != oids.size());
}

bool EventsTableModel::removeRow(int rowIndex, const QModelIndex& parent)
{
    return removeRows(rowIndex, 1, parent);
}

bool EventsTableModel::removeRows(int rowIndex, int count, const QModelIndex& parent)
{
    qDebug() << "rowIndex: " << rowIndex << ", count: " << count << ", rowCount: " << d->rowCount;

    int failures = 0;

    beginRemoveRows(parent, rowIndex, rowIndex + count - 1);

    for (int i = rowIndex; i < rowIndex + count; i++)
    {
        d->guaranteeRange(i);

        QHash< QString, QVariant > record = d->dataByRowIndex.value(i);

        if (!d->removeOid(record.value("ID").toInt()))
            failures++;
    }

    d->clearCache();
    emit rowCountChanged();

    endRemoveRows();

    return (failures != count);
}

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
