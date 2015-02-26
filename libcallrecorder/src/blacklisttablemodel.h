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

#ifndef LIBCALLRECORDER_BLACKLISTTABLEMODEL_H
#define LIBCALLRECORDER_BLACKLISTTABLEMODEL_H

#include <QAbstractListModel>

class Database;

class BlackListTableModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit BlackListTableModel(Database* db, QObject *parent = 0);
    virtual ~BlackListTableModel();

    bool contains(const QString& lineIdentification);

    QVariant data(const QModelIndex& index, int role) const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

signals:

public slots:

private:
    class BlackListTableModelPrivate;
    BlackListTableModelPrivate* d;
};

#endif // LIBCALLRECORDER_BLACKLISTTABLEMODEL_H
