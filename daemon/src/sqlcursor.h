#ifndef HARBOUR_CALLRECORDERD_SQLCURSOR_H
#define HARBOUR_CALLRECORDERD_SQLCURSOR_H

#include <QScopedPointer>
#include <QString>
#include <QVariant>

class QSqlQuery;

class SqlCursor
{
    Q_DISABLE_COPY(SqlCursor)

public:
    explicit SqlCursor(const QSqlQuery& query);
    ~SqlCursor();

    bool next();

    QVariant value(const QString& fieldName);

private:
    class SqlCursorPrivate;
    QScopedPointer< SqlCursorPrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_SQLCURSOR_H
