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
#include <QSqlError>
#include <QSqlRecord>

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

    setEditStrategy(QSqlTableModel::OnRowChange);

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
