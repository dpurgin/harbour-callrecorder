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

#ifndef HARBOUR_CALLBLOCKERD_EVENTSTABLEMODEL_H
#define HARBOUR_CALLBLOCKERD_EVENTSTABLEMODEL_H

#include <QDateTime>
#include <QScopedPointer>
#include <QVariantMap>

class EventsTableModel
{
public:
    enum RecordingState
    {
        Armed = 1,
        InProgress,
        Suspended,
        Done
    };

    enum EventType
    {
        Incoming = 1,
        Outgoing,
        Partial
    };

public:
    EventsTableModel();
    virtual ~EventsTableModel();

    int add(const QDateTime& timeStamp, int phoneNumberId, EventType eventType, RecordingState recordingState);
    void remove(int id);
    void update(int id, const QVariantMap& items);

private:
    class EventsTableModelPrivate;
    QScopedPointer< EventsTableModelPrivate > d;
};

#endif // HARBOUR_CALLBLOCKERD_EVENTSTABLEMODEL_H
