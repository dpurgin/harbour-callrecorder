#include "phonenumberstablemodel.h"

#include <QDebug>

#include "application.h"
#include "callrecorderexception.h"
#include "database.h"
#include "sqlcursor.h"

class PhoneNumbersTableModel::PhoneNumbersTableModelPrivate
{
    friend class PhoneNumbersTableModel;
};

PhoneNumbersTableModel::PhoneNumbersTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    qDebug() << __PRETTY_FUNCTION__;
}

PhoneNumbersTableModel::~PhoneNumbersTableModel()
{
    qDebug() << __PRETTY_FUNCTION__;
}

int PhoneNumbersTableModel::rowCount() const
{
    qDebug() << __PRETTY_FUNCTION__;

    Database* db = app->database();

    static QString selectStatement("SELECT COUNT(ID) AS Cnt FROM PhoneNumbers;");

    QScopedPointer< SqlCursor > cursor(db->select(selectStatement));

    if (cursor.isNull() || !cursor->next())
        throw CallRecorderException(QLatin1String("Unable to retrieve row count from PhoneNumbers"));

    return cursor->value("Cnt").toInt();
}

QVariant PhoneNumbersTableModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < rowCount() && index.column() < columnCount())
    {

    }

    return QVariant();
}

