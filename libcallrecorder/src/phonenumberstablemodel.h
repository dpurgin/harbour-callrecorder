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

#ifndef LIBCALLRECORDER_PHONENUMBERSTABLEMODEL_H
#define LIBCALLRECORDER_PHONENUMBERSTABLEMODEL_H

#include <QAbstractListModel>

class Database;

class PhoneNumbersTableModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PhoneNumbersTableModel(Database* db, QObject *parent = 0);
    virtual ~PhoneNumbersTableModel();

    Q_INVOKABLE bool contains(const QString& lineIdentification);

    QVariant data(const QModelIndex& index, int role) const;

    Q_INVOKABLE int getIdByLineIdentification(const QString& lineIdentification);

    int rowCount(const QModelIndex&) const;

signals:

public slots:

private:
    class PhoneNumbersTableModelPrivate;
    PhoneNumbersTableModelPrivate* const d;
};

#endif // LIBCALLRECORDER_PHONENUMBERSTABLEMODEL_H
