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

#include <QDate>
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
#include "settings.h"
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
        qDebug();

        dataByRowIndex.clear();
        oidToRowIndex.clear();

        rowCount = 0;

        static const QString stmt(
            "\nSELECT"
            "\n    COUNT(Events.ID) AS RowCount"
            "\nFROM"
            "\n    Events"
            "\n    LEFT JOIN"
            "\n        PhoneNumbers"
            "\n    ON"
            "\n        PhoneNumbers.ID = Events.PhoneNumberID"
            "\n%1");

        QString whereClause;
        Database::SqlParameters params;

        makeWhereClause(&whereClause, &params);

        QString selectStmt = stmt.arg(whereClause);

        QScopedPointer< SqlCursor > cursor(db->select(selectStmt, params));

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

            static const QString stmt(
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
                        "\n%1"
                        "\nORDER BY"
                        "\n    Events.TimeStamp DESC"
                        "\nLIMIT :pageSize"
                        "\nOFFSET :pageStart");

            QString whereClause;
            Database::SqlParameters params;

            makeWhereClause(&whereClause, &params);

            QString selectStmt = stmt.arg(whereClause);

            params.insert(":pageSize", pageSize);
            params.insert(":pageStart", pageStart);

            QScopedPointer< SqlCursor > cursor(db->select(selectStmt, params));

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

    void makeWhereClause(QString* whereClause, Database::SqlParameters* params)
    {
        qDebug();

        QStringList whereStmts;

        if (filters.contains(EventsTableModel::LineIdentification))
        {
            QString lineIdentificationPattern =
                    QLatin1Char('%') %
                    filters.value(EventsTableModel::LineIdentification).toString() %
                    QLatin1Char('%');

            whereStmts << "(PhoneNumbers.LineIdentification LIKE :lineIdentification)";
            params->insert(":lineIdentification", lineIdentificationPattern);
        }

        if (filters.contains(EventsTableModel::TimeStampOn))
        {
            whereStmts << "(Events.TimeStamp >= :timeStampAfter and"
                          " Events.TimeStamp < :timeStampBefore)";

            QDate dt = filters.value(EventsTableModel::TimeStampOn).toDate();

            params->insert(":timeStampAfter", dt.toString(Qt::ISODate));

            params->insert(":timeStampBefore", dt.addDays(1).toString(Qt::ISODate));
        }

        if (filters.contains(EventsTableModel::TimeStampAfter))
        {
            whereStmts << "(Events.TimeStamp >= :timeStampAfter)";

            params->insert(":timeStampAfter",
                           filters.value(EventsTableModel::TimeStampAfter).toString());
        }

        if (filters.contains(EventsTableModel::TimeStampBefore))
        {
            whereStmts << "(Events.TimeStamp < :timeStampBefore)";

            params->insert(":timeStampBefore",
                           filters.value(EventsTableModel::TimeStampBefore).toString());
        }

        if (whereStmts.size() > 0)
            *whereClause = QLatin1String("WHERE\n") % whereStmts.join(QLatin1String(" and "));
    }

    bool removeAll()
    {
        qDebug();

        bool result = true;

        static const QString staticDeleteStmt(
                    "\nDELETE"
                    "\nFROM"
                    "\n    Events"
                    "\nFROM"
                    "\n    Events"
                    "\n    LEFT JOIN"
                    "\n        PhoneNumbers"
                    "\n    ON"
                    "\n        PhoneNumbers.ID = Events.PhoneNumberID"
                    "\n%1");

        QDir outputLocationDir(Settings().outputLocation());

        // if data is not filtered, it's safe to remove all files in the Output location.
        // Otherwise we need to get list of files that meet filter criteria.

        QString whereClause;
        Database::SqlParameters params;

        makeWhereClause(&whereClause, &params);

        QString deleteStmt = staticDeleteStmt.arg(whereClause);

        if (filters.size() > 0)
        {
            qDebug() << QLatin1String("Data is filtered, removing filtered files only");

            static const QString staticSelectStmt(
                        "\nSELECT"
                        "\n    Events.FileName"
                        "\nFROM"
                        "\n    Events"
                        "\n    LEFT JOIN"
                        "\n        PhoneNumbers"
                        "\n    ON"
                        "\n        PhoneNumbers.ID = Events.PhoneNumberID"
                        "\n%1");

            QString stmt = staticSelectStmt.arg(whereClause);

            QScopedPointer< SqlCursor > cursor(db->select(stmt, params));

            // remove the filtered files now to avoid memory consumption
            // in case if query result is large.

            while (cursor->next())
            {
                QString fileName = cursor->value("FileName").toString();

                qDebug() << QLatin1String("removing") << fileName;

                outputLocationDir.remove(fileName);
            }

            // now delete from DB. In case of error we're in a bad situtation
            // where files are already removed but DB records aren't.

            db->execute(deleteStmt, params);
        }
        // data is not filtered, safe to remove everything
        else
        {
            // with non-filtered data we can check if DB is cleaned up before removing files
            if (db->execute(deleteStmt, params))
            {
                QStringList files = outputLocationDir.entryList(QDir::Files | QDir::NoDotAndDotDot);

                foreach (QString file, files)
                {
                    qDebug() << "removing" << file;
                    QFile(file).remove();
                }
            }
            else
                result = false;
        }

        return result;
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
            Settings settings;

            QString location = settings.outputLocation() % QLatin1Char('/') % fileName;

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

            qDebug() << "Error removing database row: " << db->lastError();
        }

        return result;
    }

private:
    Database* db;

    QHash< int, QHash< QString, QVariant > > dataByRowIndex;
    QHash< int, int > oidToRowIndex;

    QHash< EventsTableModel::Filter, QVariant > filters;

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
    qDebug();

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

void EventsTableModel::filter(const QVariantMap& filters)
{
    qDebug();

    d->filters.clear();

    for (QVariantMap::const_iterator cit = filters.constBegin();
         cit != filters.constEnd();
         cit++)
    {
        if (cit.key() == QLatin1String("phoneNumber"))
            d->filters.insert(LineIdentification, cit.value());
        else if (cit.key() == QLatin1String("onDate"))
            d->filters.insert(TimeStampOn, cit.value());
        else if (cit.key() == QLatin1String("beforeDate"))
            d->filters.insert(TimeStampBefore, cit.value());
        else if (cit.key() == QLatin1String("afterDate"))
            d->filters.insert(TimeStampAfter, cit.value());
    }

    refresh();
}

void EventsTableModel::refresh()
{
    qDebug();

    emit beginResetModel();

    d->clearCache();    

    emit endResetModel();

    emit rowCountChanged();
}

bool EventsTableModel::removeAll()
{
    bool result = true;

    qDebug() << "removing all";

    if (removeAll())
        refresh();
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
