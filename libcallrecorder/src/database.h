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

#ifndef LIBCALLRECORDER_DATABASE_H
#define LIBCALLRECORDER_DATABASE_H

#include <QHash>
#include <QScopedPointer>
#include <QString>
#include <QVariant>

#include "config.h"

class SqlCursor;

class LIBCALLRECORDER_DECL Database
{
    Q_DISABLE_COPY(Database)

public:
    typedef QHash< QString, QVariant > SqlParameters;

public:
    Database();
    ~Database();

public:
    bool execute(const QString& statement, const SqlParameters& params = SqlParameters());
    int insert(const QString& statement, const SqlParameters& params);
    SqlCursor* select(const QString& statement, const SqlParameters& params = SqlParameters());

    QString lastError() const;

    QStringList tableColumns(const QString& tableName);
private:

private:
    class DatabasePrivate;
    QScopedPointer< DatabasePrivate > d;
};

#endif // LIBCALLRECORDER_DATABASE_H
