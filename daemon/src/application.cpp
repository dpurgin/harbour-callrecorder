#include "application.h"

#include <qofono-qt5/qofonomanager.h>
#include <qofono-qt5/qofonovoicecallmanager.h>

#include "callrecorderexception.h"
#include "database.h"
#include "model.h"
#include "settings.h"
#include "voicecallrecorder.h"

class Application::ApplicationPrivate
{
    Q_DISABLE_COPY(ApplicationPrivate)

    friend class Application;

    explicit ApplicationPrivate(): active(true) {}

    bool active;

    QScopedPointer< Database > database;

    QScopedPointer< Model > model;

    QScopedPointer< QOfonoVoiceCallManager > qofonoVoiceCallManager;

    QScopedPointer< Settings > settings;

    // stores object paths and its recorders
    QHash< QString, VoiceCallRecorder* > voiceCallRecorders;
};

Application::Application(int argc, char* argv[])
    : QCoreApplication(argc, argv),
      d(new ApplicationPrivate())
{
    qDebug() << __PRETTY_FUNCTION__;

    setApplicationName(QLatin1String("harbour-callrecorder"));
    setOrganizationName(QLatin1String("kz.dpurgin"));

    QScopedPointer< QOfonoManager > qofonoManager(new QOfonoManager());

    if (!qofonoManager->available())
        throw CallRecorderException(QLatin1String("Ofono is not available!"));

    // check if modems available
    // use the first one for now

    QStringList modems = qofonoManager->modems();

    if (modems.length() == 0)
        throw CallRecorderException(QLatin1String("No modems available!"));

    d->database.reset(new Database());

    d->model.reset(new Model());

    d->qofonoVoiceCallManager.reset(new QOfonoVoiceCallManager());

    d->qofonoVoiceCallManager->setModemPath(modems.first());

    // connect to voice call signals from Ofono voice call manager

    connect(d->qofonoVoiceCallManager.data(), SIGNAL(callAdded(QString)),
            this, SLOT(onVoiceCallAdded(QString)));
    connect(d->qofonoVoiceCallManager.data(), SIGNAL(callRemoved(QString)),
            this, SLOT(onVoiceCallRemoved(QString)));

    d->settings.reset(new Settings());
}

Application::~Application()
{
    qDebug() << __PRETTY_FUNCTION__;
}

/// Checks whether the call recording application is active
bool Application::active() const
{
    return d->active;
}

Database* Application::database() const
{
    return d->database.data();
}

Model* Application::model() const
{
    return d->model.data();
}

void Application::onVoiceCallAdded(const QString& objectPath)
{
    qDebug() << __PRETTY_FUNCTION__ << objectPath;

    if (active())
    {
        QScopedPointer< VoiceCallRecorder > voiceCallRecorder(new VoiceCallRecorder(objectPath));
        d->voiceCallRecorders.insert(objectPath, voiceCallRecorder.take());
    }
}

void Application::onVoiceCallRemoved(const QString& objectPath)
{
    qDebug() << __PRETTY_FUNCTION__ << objectPath;

    // not checking active() here, because the call recorder could be deactivated while recording a call

    VoiceCallRecorder* voiceCallRecorder = d->voiceCallRecorders.value(objectPath, NULL);

    // clean up if call recorder exists

    if (voiceCallRecorder)
    {
        delete voiceCallRecorder;
        d->voiceCallRecorders.remove(objectPath);
    }
}

Settings* Application::settings() const
{
    return d->settings.data();
}
