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

#include "blacklisttablemodel.h"

#include <QDebug>

#include "database.h"
#include "sqlcursor.h"

class BlackListTableModel::BlackListTableModelPrivate
{
    friend class BlackListTableModel;

    BlackListTableModelPrivate()
        : db(NULL)
    {
    }

    Database* db;
};

BlackListTableModel::BlackListTableModel(Database* db, QObject* parent)
    : QAbstractListModel(parent),
      d(new BlackListTableModelPrivate())
{
    d->db = db;
}

BlackListTableModel::~BlackListTableModel()
{
    delete d;
}

bool BlackListTableModel::contains(const QString& lineIdentification)
{
    bool result = false;

    static QString stmt(
                "\nSELECT"
                "\n    BlackList.ID"
                "\nFROM"
                "\n    BlackList"
                "\n"
                "\n    LEFT JOIN"
                "\n        PhoneNumbers"
                "\n    ON"
                "\n        PhoneNumbers.ID = BlackList.PhoneNumberID"
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

QVariant BlackListTableModel::data(const QModelIndex& index, int role) const
{
    return QVariant();
}

int BlackListTableModel::rowCount(const QModelIndex& parent) const
{
    return 0;
}
