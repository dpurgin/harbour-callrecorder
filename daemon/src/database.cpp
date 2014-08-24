#include "database.h"

#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QStringBuilder>

#include "callrecorderexception.h"
#include "sqlcursor.h"

class Database::DatabasePrivate
{
    friend class Database;

private:
    bool prepareAndExecute(const QString& statement, const SqlParameters& params, QSqlQuery* query = NULL)
    {
        qDebug() << __PRETTY_FUNCTION__ << statement << params;

        resetLastError();

        if (!query->prepare(statement))
        {
            QString errorText = QLatin1String("Unable to prepare query: ") % query->lastError().text();

            qDebug() << __PRETTY_FUNCTION__ << errorText;

            setLastError(errorText);
            return false;
        }

        for (SqlParameters::const_iterator cit = params.cbegin();
             cit != params.cend();
             cit++)
        {
            query->bindValue(cit.key(), cit.value());
        }

        if (!query->exec())
        {
            QString errorText = QLatin1String("Unable to execute query: ") % query->lastError().text();

            qDebug() << __PRETTY_FUNCTION__ << errorText;

            setLastError(errorText);

            return false;
        }

        return true;

    }

    void resetLastError() { setLastError(QLatin1String("")); }
    void setLastError(const QString& error) { lastError = error; }

private:
    QScopedPointer< QSqlDatabase > db;
    QString lastError;
};

Database::Database()
    : d(new DatabasePrivate())
{
    qDebug() << __PRETTY_FUNCTION__;

    d->db.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE")));

    QString location = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

    QDir locationDir(location);

    if (!locationDir.exists())
    {
        qDebug() << "Trying to create " << location;

        if (!locationDir.mkpath(locationDir.absolutePath()))
            throw CallRecorderException(QLatin1String("Unable to make path: ") % location);
    }

    qDebug() << "Opening database " << location % QLatin1String("/callrecorder.db");

    d->db->setDatabaseName(location % QLatin1String("/callrecorder.db"));

    if (!d->db->open())
        throw CallRecorderException(QLatin1String("Unable to open database: ") % d->db->lastError().text());

    static QStringList initStatements = QStringList() <<
        "\nPRAGMA foreign_keys = ON;" <<

        "\nCREATE TABLE IF NOT EXISTS PhoneNumbers(ID INTEGER PRIMARY KEY AUTOINCREMENT, LineIdentification TEXT);" <<

        "\nINSERT OR IGNORE INTO PhoneNumbers(ID, LineIdentification) VALUES(1, '');" <<

        "\nCREATE TABLE IF NOT EXISTS Events"
        "\n("
        "\n    ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "\n    TimeStamp TEXT,"
        "\n    PhoneNumberID INTEGER,"
        "\n    EventTypeID INTEGER,"
        "\n    RecordingStateID INTEGER,"
        "\n    FileName TEXT,"
        "\n    FileSize INTEGER,"
        "\n    FOREIGN KEY(PhoneNumberID) REFERENCES PhoneNumbers(ID)"
        "\n);";

    foreach (QString statement, initStatements)
    {
        if (!execute(statement))
            throw CallRecorderException(QLatin1String("Unable to execute initializing statement: ") %
                                        statement %
                                        lastError());
    }
}

Database::~Database()
{
    qDebug() << __PRETTY_FUNCTION__;

    if (d->db->isOpen())
        d->db->close();
}

bool Database::execute(const QString& statement, const SqlParameters& params)
{
    qDebug() << __PRETTY_FUNCTION__ << statement;

    QSqlQuery query(*d->db.data());    

    return d->prepareAndExecute(statement, params, &query);
}

int Database::insert(const QString& statement, const SqlParameters& params)
{
    QSqlQuery query(*d->db.data());

    if (!d->prepareAndExecute(statement, params, &query))
        return -1;

    return query.lastInsertId().toInt();
}

QString Database::lastError() const
{
    return d->lastError;
}

SqlCursor* Database::select(const QString& statement, const SqlParameters& params)
{
    QSqlQuery query(*d->db.data());

    if (!d->prepareAndExecute(statement, params, &query))
        return NULL;

    return new SqlCursor(query);
}
