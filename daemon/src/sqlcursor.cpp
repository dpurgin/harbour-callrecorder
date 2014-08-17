#include "sqlcursor.h"

#include <QSqlQuery>

class SqlCursor::SqlCursorPrivate
{
    friend class SqlCursor;

    QSqlQuery query;
};

SqlCursor::SqlCursor(const QSqlQuery &query)
    : d(new SqlCursorPrivate())
{
    d->query = query;
}

SqlCursor::~SqlCursor()
{

}

bool SqlCursor::next()
{
    return d->query.next();
}

QVariant SqlCursor::value(const QString& fieldName)
{
    return d->query.value(fieldName);
}
