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
#include <QSqlError>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QStringBuilder>

#include "database.h"

class EventsTableModel::EventsTableModelPrivate
{
    friend class EventsTableModel;

    EventsTableModelPrivate()
        : db(NULL)
    {
    }

    Database* db;
};

EventsTableModel::EventsTableModel(Database* db, QObject* parent)
    : QSqlRelationalTableModel(parent),
      d(new EventsTableModelPrivate())
{
    qDebug() << __PRETTY_FUNCTION__;

    d->db = db;

    setTable("Events");

    setSort(1, Qt::DescendingOrder);

    setRelation(2, QSqlRelation("PhoneNumbers", "ID", "LineIdentification"));

    setEditStrategy(QSqlTableModel::OnFieldChange);

    if (!select())
        qDebug() << __PRETTY_FUNCTION__ << ": unable to perform select: " << lastError().text();
    else
        qDebug() << __PRETTY_FUNCTION__ << ": " << rowCount() << " selected";    
}

EventsTableModel::~EventsTableModel()
{
    qDebug() << __PRETTY_FUNCTION__;
}

QVariant EventsTableModel::data(const QModelIndex& item, int role) const
{    
    return QSqlRelationalTableModel::data(index(item.row(), role - Qt::UserRole));
}

bool EventsTableModel::removeItem(const QString& id, const QString& fileName)
{
    qDebug() << "id: " << id << ", fileName: " << fileName;

    Database::SqlParameters params;
    params.insert(":id", id);

    if (d->db->execute("DELETE FROM Events WHERE ID = :id", params))
    {
        QString location = QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
                QLatin1String("/data/") %
                fileName;

        qDebug() << "removing" << location;

        QFile(location).remove();

        select();
    }
    else
        qDebug() << "error removing item: " << d->db->lastError();

    return true;
}

QHash< int, QByteArray > EventsTableModel::roleNames() const
{
    static QHash< int, QByteArray > roles;

    if (roles.empty())
    {
        QSqlRecord rec = record();

        for (int c = 0; c < rec.count(); c++)
            roles.insert(Qt::UserRole + c, rec.fieldName(c).toUtf8());
    }

    qDebug() << roles;

    return roles;
}

bool EventsTableModel::select()
{
    qDebug() << "";

    int oldRowCount = rowCount();

    bool result = QSqlRelationalTableModel::select();

    if (rowCount() != oldRowCount)
        emit rowCountChanged();

    return result;
}
