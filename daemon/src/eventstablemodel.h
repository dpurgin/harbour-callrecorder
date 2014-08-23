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
        Outgoing
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
