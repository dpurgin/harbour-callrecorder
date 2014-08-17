#ifndef HARBOUR_CALLRECORDERD_PHONENUMBERSTABLEMODEL_H
#define HARBOUR_CALLRECORDERD_PHONENUMBERSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QScopedPointer>

class PhoneNumbersTableModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_DISABLE_COPY(PhoneNumbersTableModel)

public:
    explicit PhoneNumbersTableModel(QObject* parent = 0);
    virtual ~PhoneNumbersTableModel();

    QVariant data(const QModelIndex& index, int role) const;

    int rowCount() const;
    int columnCount() const { return 2; }


signals:

public slots:

private:
    class PhoneNumbersTableModelPrivate;
    QScopedPointer< PhoneNumbersTableModelPrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_PHONENUMBERSTABLEMODEL_H
