#ifndef HARBOUR_CALLRECORDERD_APPLICATION_H
#define HARBOUR_CALLRECORDERD_APPLICATION_H

#include <QCoreApplication>
#include <QScopedPointer>

#define app (reinterpret_cast< Application* >(qApp))

class Database;
class Settings;

class Application : public QCoreApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(Application)

public:
    explicit Application(int argc, char* argv[]);
    virtual ~Application();

    bool active() const;
    void setActive(bool active);

    Database* database() const;
    Settings* settings() const;

signals:   

public slots:

private slots:
    void onVoiceCallAdded(const QString& objectPath);
    void onVoiceCallRemoved(const QString& objectPath);


private:
    class ApplicationPrivate;
    QScopedPointer< ApplicationPrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_APPLICATION_H
