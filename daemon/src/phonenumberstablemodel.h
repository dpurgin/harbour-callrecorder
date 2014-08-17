#ifndef HARBOUR_CALLRECORDERD_PHONENUMBERSTABLEMODEL_H
#define HARBOUR_CALLRECORDERD_PHONENUMBERSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QScopedPointer>

class PhoneNumbersTableModel
{
    Q_DISABLE_COPY(PhoneNumbersTableModel)

public:
    explicit PhoneNumbersTableModel();
    ~PhoneNumbersTableModel();

    int getIdByLineIdentification(const QString& lineIdentification);

signals:

public slots:

private:
    class PhoneNumbersTableModelPrivate;
    QScopedPointer< PhoneNumbersTableModelPrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_PHONENUMBERSTABLEMODEL_H
