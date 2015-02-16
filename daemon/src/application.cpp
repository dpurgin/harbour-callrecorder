/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2015 Dmitriy Purgin <dpurgin@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "application.h"

#include <QTimer>

#include <qofonomanager.h>
#include <qofonovoicecallmanager.h>

#include <libcallrecorder/callrecorderexception.h>
#include <libcallrecorder/database.h>
#include <libcallrecorder/settings.h>

#include <qtpulseaudio/qtpulseaudioconnection.h>
#include <qtpulseaudio/qtpulseaudiocard.h>
#include <qtpulseaudio/qtpulseaudiocardprofile.h>
#include <qtpulseaudio/qtpulseaudioserver.h>
#include <qtpulseaudio/qtpulseaudiosink.h>
#include <qtpulseaudio/qtpulseaudiosinkport.h>
#include <qtpulseaudio/qtpulseaudiosource.h>

#include "dbusadaptor.h"
#include "uidbusinterface.h"
#include "model.h"
#include "voicecallrecorder.h"

class Application::ApplicationPrivate
{
    Q_DISABLE_COPY(ApplicationPrivate)

    friend class Application;

private:
    explicit ApplicationPrivate()
        : active(false),
          pulseAudioCard(NULL),
          wantPark(false),
          needResetDefaultSource(false)
    {}

private:
    bool active;

    QScopedPointer< Database > database;

    QScopedPointer< DBusAdaptor > dbusAdaptor;
    QScopedPointer< UiDBusInterface > uiInterface;

    QScopedPointer< Model > model;

    QSharedPointer< QtPulseAudioCard > pulseAudioCard;
    QSharedPointer< QtPulseAudioSink > pulseAudioSink;
    QScopedPointer< QtPulseAudioConnection > pulseAudioConnection;

    QScopedPointer< QOfonoManager > qofonoManager;
    QScopedPointer< QOfonoVoiceCallManager > qofonoVoiceCallManager;

    QScopedPointer< Settings > settings;

    QScopedPointer< QTimer > timer;

    // stores object paths and its recorders
    QHash< QString, VoiceCallRecorder* > voiceCallRecorders;

    bool wantPark;
    QString wantPort;

    // workaround for Android mic issue
    bool needResetDefaultSource;
};


Application::Application(int argc, char* argv[])
    : QCoreApplication(argc, argv),
      d(new ApplicationPrivate())
{
    qDebug() << __PRETTY_FUNCTION__;

    setApplicationName(QLatin1String("harbour-callrecorder"));
    setOrganizationName(QLatin1String("kz.dpurgin"));

    d->settings.reset(new Settings());

    d->dbusAdaptor.reset(new DBusAdaptor(this));
    d->uiInterface.reset(new UiDBusInterface(this));

    connect(d->uiInterface.data(), SIGNAL(SettingsChanged()),
            d->settings.data(), SLOT(reload()));

    d->database.reset(new Database());
    d->model.reset(new Model());

    d->qofonoManager.reset(new QOfonoManager());

    if (!d->qofonoManager->available())
        throw CallRecorderException(QLatin1String("Ofono is not available!"));

    // check if modems available. If there are modems, take the first one and initialize app with it
    // If no modems available, wait for first modemAdded and initialize with this one

    QStringList modems = d->qofonoManager->modems();

    if (modems.length() == 0)
    {
        qWarning() << QLatin1String("No modems available! Waiting for modemAdded");

        connect(d->qofonoManager.data(), SIGNAL(modemAdded(QString)),
                this, SLOT(initVoiceCallManager(QString)));
    }
    else
        initVoiceCallManager(modems.first());

    d->pulseAudioConnection.reset(
        new QtPulseAudioConnection(QtPulseAudio::Card |
                                   QtPulseAudio::Server |
                                   QtPulseAudio::Source |
                                   QtPulseAudio::Sink));

    connect(d->pulseAudioConnection.data(), SIGNAL(connected()),
            this, SLOT(onPulseAudioConnected()));
    connect(d->pulseAudioConnection.data(), SIGNAL(error(QString)),
            this, SLOT(onPulseAudioError(QString)));

    d->pulseAudioConnection->connectToServer();
}

Application::~Application()
{
    qDebug() << __PRETTY_FUNCTION__;
}

Database* Application::database() const
{
    return d->database.data();
}

Model* Application::model() const
{
    return d->model.data();
}

/// Connects to Ofono voice call add/remove signals using the modem given
void Application::initVoiceCallManager(const QString& objectPath)
{
    qDebug() << __PRETTY_FUNCTION__ << objectPath;

    d->qofonoVoiceCallManager.reset(new QOfonoVoiceCallManager());

    d->qofonoVoiceCallManager->setModemPath(objectPath);

    // connect to voice call signals from Ofono voice call manager

    connect(d->qofonoVoiceCallManager.data(), SIGNAL(callAdded(QString)),
            this, SLOT(onVoiceCallAdded(QString)));
    connect(d->qofonoVoiceCallManager.data(), SIGNAL(callRemoved(QString)),
            this, SLOT(onVoiceCallRemoved(QString)));

    // check if there are any active calls on start
    QStringList activeCalls = d->qofonoVoiceCallManager->getCalls();

    foreach (QString call, activeCalls)
        onVoiceCallAdded(call);

    // ofono manager is not needed now
    d->qofonoManager->deleteLater();
}

void Application::maybeSwitchProfile()
{
    qDebug() << QThread::currentThread() <<
                "Card profile: " << d->pulseAudioCard->activeProfile()->name() <<
                ", sink port: " << d->pulseAudioSink->activePort()->name() <<
                ", want park: " << d->wantPark <<
                ", want port: " << d->wantPort;

    if (d->pulseAudioCard->activeProfile()->name() == QLatin1String("voicecall"))
    {
        qDebug() << "Switching profile to voicecall-record";

        d->pulseAudioCard->setActiveProfile(QLatin1String("voicecall-record"));              

        d->wantPark = true;
        d->wantPort = d->pulseAudioSink->activePort()->name();

        qDebug() << "Now want park and port" << d->wantPort;
    }
    else if (d->pulseAudioCard->activeProfile()->name() == QLatin1String("voicecall-record"))
    {
        if (d->pulseAudioSink->activePort()->name() == QLatin1String("output-parking"))
        {
            qDebug() << "Port parked -- switching to " << d->wantPort;

            d->pulseAudioSink->setActivePort(d->wantPort);
        }
        else if (d->wantPark || d->pulseAudioSink->activePort()->name() != d->wantPort)
        {
            qDebug() << "Now want port " << d->pulseAudioSink->activePort()->name();

            d->wantPark = false;
            d->wantPort = d->pulseAudioSink->activePort()->name();
            d->pulseAudioSink->setActivePort(QLatin1String("output-parking"));
        }
    }
    else
        qDebug() << "Not managing profile " << d->pulseAudioCard->activeProfile()->name();
}

void Application::onPulseAudioCardActiveProfileChanged(QString profileName)
{
    if (d->active)
    {
        qDebug() << QThread::currentThread() << ": active profile: " << profileName;

        maybeSwitchProfile();
    }
}

void Application::onPulseAudioConnected()
{
    qDebug() << QThread::currentThread() << ": connected to PulseAudio";

    d->timer.reset(new QTimer());
    d->timer->setInterval(250);
    d->timer->setSingleShot(true);
    connect(d->timer.data(), SIGNAL(timeout()),
            this, SLOT(maybeSwitchProfile()));

    d->pulseAudioCard = d->pulseAudioConnection->cardByIndex(0);
    d->pulseAudioSink = d->pulseAudioConnection->sinkByName("sink.primary");

    if (!d->pulseAudioCard.isNull())
    {
        connect(d->pulseAudioCard.data(), SIGNAL(activeProfileChanged(QString)),
                this, SLOT(onPulseAudioCardActiveProfileChanged(QString)));
    }
    else
        qDebug() << "no PulseAudio cards found!";


    if (!d->pulseAudioSink.isNull())
    {
        connect(d->pulseAudioSink.data(), SIGNAL(activePortChanged(QString)),
                this, SLOT(onPulseAudioSinkActivePortChanged(QString)));
    }
    else
        qDebug() << "no PulseAudio sink named sink.primary found!";

    connect(d->pulseAudioConnection.data(), SIGNAL(sourceAdded(QSharedPointer<QtPulseAudioSource>)),
            this, SLOT(onPulseAudioSourceAdded(QSharedPointer<QtPulseAudioSource>)));

    connect(d->pulseAudioConnection.data(), SIGNAL(sourceRemoved(QSharedPointer<QtPulseAudioSource>)),
            this, SLOT(onPulseAudioSourceRemoved(QSharedPointer<QtPulseAudioSource>)));
}

void Application::onPulseAudioError(QString error)
{
    qDebug() << QThread::currentThread() << ": error connecting to PulseAudio: " << error;

    quit();
}

void Application::onPulseAudioSinkActivePortChanged(QString portName)
{
    if (d->active)
    {
        qDebug() << QThread::currentThread() << ": active port: " << portName;

        maybeSwitchProfile();
    }
}

void Application::onPulseAudioSourceAdded(QSharedPointer< QtPulseAudioSource > source)
{
    qDebug() << QThread::currentThread() << source->index() << source->name();

    // Workaround for Android mic issue.
    // If recording is active, default source is being reset constantly.
    // After recording is done, the app leaves "needResetDefaultSource" flag in case if
    // source.primary would be recreated after recording is done
    if ((d->active || d->needResetDefaultSource) &&
            source->name() == QLatin1String("source.primary"))
    {
        d->pulseAudioConnection->server()->setDefaultSource("source.primary");

        if (!d->active && d->needResetDefaultSource)
            d->needResetDefaultSource = false;
    }
}

void Application::onPulseAudioSourceRemoved(QSharedPointer< QtPulseAudioSource > source)
{
    qDebug() << QThread::currentThread() << source->index() << source->name();
}

/// Creates the recorder for a voice call appeared in the system
void Application::onVoiceCallAdded(const QString& objectPath)
{
    qDebug() << __PRETTY_FUNCTION__ << objectPath;

    QScopedPointer< VoiceCallRecorder > voiceCallRecorder(new VoiceCallRecorder(objectPath));

    connect(voiceCallRecorder.data(), SIGNAL(stateChanged(VoiceCallRecorder::State)),
            this, SLOT(onVoiceCallRecorderStateChanged(VoiceCallRecorder::State)));
    connect(voiceCallRecorder.data(), SIGNAL(stateChanged(VoiceCallRecorder::State)),
            d->dbusAdaptor.data(), SIGNAL(RecorderStateChanged()));

    voiceCallRecorder->processState();

    d->voiceCallRecorders.insert(objectPath, voiceCallRecorder.take());        
}

void Application::onVoiceCallRecorderStateChanged(VoiceCallRecorder::State state)
{
    qDebug() << state;

    if (state == VoiceCallRecorder::Active)
    {
        d->active = true;

        maybeSwitchProfile();
    }
    else
        d->active = false;
}

/// Checks whether recorder is active and cleans it up after the voice call was removed from the system
void Application::onVoiceCallRemoved(const QString& objectPath)
{
    qDebug() << __PRETTY_FUNCTION__ << objectPath;

    VoiceCallRecorder* voiceCallRecorder = d->voiceCallRecorders.value(objectPath, NULL);

    // clean up if call recorder exists

    if (voiceCallRecorder)
    {
        delete voiceCallRecorder;
        d->voiceCallRecorders.remove(objectPath);

        // workaround for Android mic issue
        d->needResetDefaultSource = true;
    }
}

Settings* Application::settings() const
{
    return d->settings.data();
}
