/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2016 Dmitriy Purgin <dpurgin@gmail.com>

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

#include "model.h"

#include <QDebug>

#include <libcallrecorder/blacklisttablemodel.h>
#include <libcallrecorder/database.h>
#include <libcallrecorder/eventstablemodel.h>
#include <libcallrecorder/phonenumberstablemodel.h>
#include <libcallrecorder/whitelisttablemodel.h>

class Model::ModelPrivate
{
    friend class Model;

    ModelPrivate(Database* db)
        : blackList(new BlackListTableModel(db)),
          events(new EventsTableModel(db)),
          phoneNumbers(new PhoneNumbersTableModel(db)),
          whiteList(new WhiteListTableModel(db))
    {
    }

    QScopedPointer< BlackListTableModel > blackList;
    QScopedPointer< EventsTableModel > events;
    QScopedPointer< PhoneNumbersTableModel > phoneNumbers;
    QScopedPointer< WhiteListTableModel > whiteList;
};

Model::Model(Database* db)
    : d(new ModelPrivate(db))
{
    qDebug();
}

Model::~Model()
{
    qDebug();
}

BlackListTableModel* Model::blackList() const
{
    return d->blackList.data();
}

EventsTableModel* Model::events() const
{
    return d->events.data();
}

PhoneNumbersTableModel* Model::phoneNumbers() const
{
    return d->phoneNumbers.data();
}

WhiteListTableModel* Model::whiteList() const
{
    return d->whiteList.data();
}
