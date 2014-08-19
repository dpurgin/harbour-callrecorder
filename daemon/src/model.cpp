#include "model.h"

#include <QDebug>

#include "eventstablemodel.h"
#include "phonenumberstablemodel.h"

class Model::ModelPrivate
{
    friend class Model;

    ModelPrivate()
        : events(new EventsTableModel()),
          phoneNumbers(new PhoneNumbersTableModel())
    {
    }

    QScopedPointer< EventsTableModel > events;
    QScopedPointer< PhoneNumbersTableModel > phoneNumbers;
};

Model::Model()
    : d(new ModelPrivate())
{
    qDebug() << __PRETTY_FUNCTION__;
}

Model::~Model()
{
    qDebug() << __PRETTY_FUNCTION__;
}

EventsTableModel* Model::events() const
{
    return d->events.data();
}

PhoneNumbersTableModel* Model::phoneNumbers() const
{
    return d->phoneNumbers.data();
}
