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

#ifndef LIBCALLRECORDER_EVENTSTABLEMODEL_H
#define LIBCALLRECORDER_EVENTSTABLEMODEL_H

#include <QAbstractListModel>
#include <QVector>

#include "config.h"

class Database;

/*! Model for Events table in the database
 */
class LIBCALLRECORDER_DECL EventsTableModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)

public:
    enum Filter
    {
        TimeStampOn,
        TimeStampBefore,
        TimeStampAfter,
        LineIdentification
    };

    enum RecordingState
    {
        Armed = 1,
        InProgress,
        Suspended,
        Done,
        WaitingForApproval
    };

    enum EventType
    {
        Incoming = 1,
        Outgoing,
        Partial
    };

public:
    EventsTableModel(Database* db, QObject* parent = 0);
    virtual ~EventsTableModel();

    QVariant data(const QModelIndex& item, int role = Qt::DisplayRole) const;

    /*! Adds a new row to the table
     *
     * Is not Q_INVOKABLE as only the daemon can call this method
     *
     * \return OID of the inserted row
     */
    int add(QDateTime timeStamp,
            int phoneNumberId,
            EventType eventTypeId,
            RecordingState recordingStateId);

    Q_INVOKABLE void filter(const QVariantMap& filters);

    Q_INVOKABLE bool remove(int oid);
    Q_INVOKABLE bool removeAll();
    Q_INVOKABLE bool removeOids(const QList< int >& oids);
    Q_INVOKABLE bool removeRows(int rowIndex, int count, const QModelIndex& = QModelIndex());
    Q_INVOKABLE bool removeRow(int rowIndex, const QModelIndex& = QModelIndex());

    Q_INVOKABLE void refresh();

    QHash< int, QByteArray > roleNames() const;

    int rowCount(const QModelIndex& = QModelIndex()) const;

    Q_INVOKABLE bool update(int oid, const QVariantMap& items);

signals:
    void rowCountChanged();

public slots:        

private:
    class EventsTableModelPrivate;
    EventsTableModelPrivate* d;
};

#endif // LIBCALLRECORDER_EVENTSMODEL_H
