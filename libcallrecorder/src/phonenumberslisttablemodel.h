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

#ifndef LIBCALLRECORDER_PHONENUMBERSLISTTABLEMODEL_H
#define LIBCALLRECORDER_PHONENUMBERSLISTTABLEMODEL_H

#include <QAbstractListModel>

class Database;

class PhoneNumbersListTableModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)

public:
    enum Columns
    {
        ID = Qt::UserRole,
        PhoneNumberID,
        PhoneNumberIDRepresentation
    };

public:
    explicit PhoneNumbersListTableModel(QString tableName,
                                        Database* db,
                                        QObject *parent = 0);
    virtual ~PhoneNumbersListTableModel();

    Q_INVOKABLE bool add(int phoneNumberId);

    Q_INVOKABLE bool contains(const QString& lineIdentification) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    QHash< int, QByteArray > roleNames() const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    bool submitAll();

    QString tableName() const;

signals:
    void rowCountChanged();

public slots:

private:
    class PhoneNumbersListTableModelPrivate;
    PhoneNumbersListTableModelPrivate* const d;
};

#endif // LIBCALLRECORDER_PHONENUMBERSLISTTABLEMODEL_H
