/*
    Call Recorder for SailfishOS
    Copyright (C) 2014  Dmitriy Purgin <dpurgin@gmail.com>

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

#include <qofono-qt5/qofonomanager.h>
#include <qofono-qt5/qofonovoicecallmanager.h>

#include <libcallrecorder/callrecorderexception.h>
#include <libcallrecorder/database.h>

#include "dbusadaptor.h"
#include "model.h"
#include "pulseaudiocard.h"
#include "pulseaudiocardprofile.h"
#include "pulseaudiosink.h"
#include "pulseaudiosinkport.h"
#include "pulseaudiowrapper.h"
#include "settings.h"
#include "voicecallrecorder.h"

class Application::ApplicationPrivate
{
    Q_DISABLE_COPY(ApplicationPrivate)

    friend class Application;

private:
    explicit ApplicationPrivate(): active(false), pulseAudioCard(NULL), wantPark(false) {}

private:
    bool active;

    QScopedPointer< Database > database;

    QScopedPointer< DBusAdaptor > dbusAdaptor;

    QScopedPointer< Model > model;

    PulseAudioCard* pulseAudioCard;
    PulseAudioSink* pulseAudioSink;
    QScopedPointer< PulseAudioWrapper > pulseAudioWrapper;

    QScopedPointer< QOfonoManager > qofonoManager;
    QScopedPointer< QOfonoVoiceCallManager > qofonoVoiceCallManager;

    QScopedPointer< Settings > settings;

    QScopedPointer< QTimer > timer;

    // stores object paths and its recorders
    QHash< QString, VoiceCallRecorder* > voiceCallRecorders;

    bool wantPark;
    QString wantPort;
};


Application::Application(int argc, char* argv[])
    : QCoreApplication(argc, argv),
      d(new ApplicationPrivate())
{
    qDebug() << __PRETTY_FUNCTION__;

    setApplicationName(QLatin1String("harbour-callrecorder"));
    setOrganizationName(QLatin1String("kz.dpurgin"));

    d->dbusAdaptor.reset(new DBusAdaptor(this));

    d->database.reset(new Database());
    d->model.reset(new Model());
    d->settings.reset(new Settings());

    d->timer.reset(new QTimer());
    d->timer->setInterval(250);
    d->timer->setSingleShot(true);
    connect(d->timer.data(), SIGNAL(timeout()),
            this, SLOT(maybeSwitchProfile()));

    d->pulseAudioWrapper.reset(new PulseAudioWrapper());
    d->pulseAudioCard = d->pulseAudioWrapper->cardByIndex(0);
    d->pulseAudioSink = d->pulseAudioWrapper->sinkByName("sink.primary");

    connect(d->pulseAudioCard, SIGNAL(activeProfileChanged(const PulseAudioCardProfile*)),
            this, SLOT(onPulseAudioCardActiveProfileChanged(const PulseAudioCardProfile*)));

    connect(d->pulseAudioSink, SIGNAL(activePortChanged(const PulseAudioSinkPort*)),
            this, SLOT(onPulseAudioSinkActivePortChanged(const PulseAudioSinkPort*)));

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
    qDebug() << "Card profile: " << d->pulseAudioCard->activeProfile()->name() <<
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
    else if (d->pulseAudioCard->activeProfile()->name() == QLatin1String("voicecall-record") &&
             (d->pulseAudioSink->activePort()->name() != d->wantPort || d->wantPark))
    {
        if (d->wantPark)
        {
            qDebug() << "Switching port to output-parking";

            d->pulseAudioSink->setActivePort("output-parking");
            d->wantPark = false;
        }
        else
        {
            qDebug() << "Switching port to " << d->wantPort;

            d->pulseAudioSink->setActivePort(d->wantPort);
        }
    }
    else
        qDebug() << "Not switching profile";
}

void Application::onPulseAudioCardActiveProfileChanged(const PulseAudioCardProfile* profile)
{
    if (d->active)
    {
        qDebug() << "Active profile: " << (profile? profile->name(): "NULL");

        if (profile && !d->timer->isActive())
            d->timer->start();
    }
}

void Application::onPulseAudioSinkActivePortChanged(const PulseAudioSinkPort* port)
{
    if (d->active)
    {
        qDebug() << "Active port: " << (port->name());

        if (port && !d->timer->isActive())
            d->timer->start();
    }
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
    qDebug();

    if (state == VoiceCallRecorder::Active)
    {
        d->active = true;

        if (!d->timer->isActive() && d->pulseAudioCard->activeProfile()->name() == QLatin1String("voicecall"))
            d->timer->start();
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
    }
}

Settings* Application::settings() const
{
    return d->settings.data();
}
