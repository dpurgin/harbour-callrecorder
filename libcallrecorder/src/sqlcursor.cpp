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

#include "sqlcursor.h"

#include <QSqlQuery>

class SqlCursor::SqlCursorPrivate
{
    friend class SqlCursor;

    QSqlQuery query;
};

SqlCursor::SqlCursor(const QSqlQuery &query)
    : d(new SqlCursorPrivate())
{
    d->query = query;
}

SqlCursor::~SqlCursor()
{

}

bool SqlCursor::next()
{
    return d->query.next();
}

QVariant SqlCursor::value(const QString& fieldName)
{
    return d->query.value(fieldName);
}
