#ifndef HARBOUR_CALLBLOCKERD_MODEL_H
#define HARBOUR_CALLBLOCKERD_MODEL_H

#include <QScopedPointer>

class EventsTableModel;
class PhoneNumbersTableModel;

class Model
{
    Q_DISABLE_COPY(Model)

public:
    Model();
    ~Model();

    EventsTableModel* events() const;
    PhoneNumbersTableModel* phoneNumbers() const;

private:
    class ModelPrivate;
    QScopedPointer< ModelPrivate > d;
};


#endif // HARBOUR_CALLBLOCKERD_MODEL_H
