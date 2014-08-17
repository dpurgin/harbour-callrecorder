#ifndef HARBOUR_CALLBLOCKERD_EVENTSTABLEMODEL_H
#define HARBOUR_CALLBLOCKERD_EVENTSTABLEMODEL_H

#include <QDateTime>
#include <QScopedPointer>
#include <QVariantHash>

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
        Outgoing
    };

public:
    EventsTableModel();
    virtual ~EventsTableModel();

    void add(const QDateTime& timeStamp, EventType eventType);

private:
    class EventsTableModelPrivate;
    QScopedPointer< EventsTableModelPrivate > d;
};

#endif // HARBOUR_CALLBLOCKERD_EVENTSTABLEMODEL_H
