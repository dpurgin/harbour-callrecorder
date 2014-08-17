#ifndef HARBOUR_CALLRECORDERD_DATABASE_H
#define HARBOUR_CALLRECORDERD_DATABASE_H

#include <QHash>
#include <QScopedPointer>
#include <QString>
#include <QVariant>

class SqlCursor;

class Database
{
    Q_DISABLE_COPY(Database)

    typedef QHash< QString, QVariant > SqlParameters;

public:
    Database();
    ~Database();

public:
    bool execute(const QString& statement, const SqlParameters& params = SqlParameters());
    SqlCursor* select(const QString& statement, const SqlParameters& params = SqlParameters());

    QString lastError() const;
private:

private:
    class DatabasePrivate;
    QScopedPointer< DatabasePrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_DATABASE_H
