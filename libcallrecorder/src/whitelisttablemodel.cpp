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

#include "whitelisttablemodel.h"

#include <QDebug>

#include "database.h"
#include "sqlcursor.h"

class WhiteListTableModel::WhiteListTableModelPrivate
{
    friend class WhiteListTableModel;

    Database* db;
};

WhiteListTableModel::WhiteListTableModel(Database* db, QObject *parent)
    : QSqlRelationalTableModel(parent),
      d(new WhiteListTableModelPrivate())
{
    d->db = db;

    setTable("WhiteList");
    setRelation(1, QSqlRelation("PhoneNumbers", "ID", "LineIdentification"));
}

WhiteListTableModel::~WhiteListTableModel()
{
    delete d;
}

bool WhiteListTableModel::contains(const QString& lineIdentification) const
{
    bool result = false;

    static QString stmt(
                "\nSELECT"
                "\n    WhiteList.ID"
                "\nFROM"
                "\n    WhiteList"
                "\n"
                "\n    LEFT JOIN"
                "\n        PhoneNumbers"
                "\n    ON"
                "\n        PhoneNumbers.ID = WhiteList.PhoneNumberID"
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
