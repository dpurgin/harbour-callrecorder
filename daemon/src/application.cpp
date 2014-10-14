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

#include <qofono-qt5/qofonomanager.h>
#include <qofono-qt5/qofonovoicecallmanager.h>

#include <libcallrecorder/callrecorderexception.h>
#include <libcallrecorder/database.h>

#include <pulse/context.h>
#include <pulse/mainloop-api.h>
#include <pulse/subscribe.h>
#include <pulse/thread-mainloop.h>

#include "model.h"
#include "settings.h"
#include "voicecallrecorder.h"

class Application::ApplicationPrivate
{
    Q_DISABLE_COPY(ApplicationPrivate)

    friend class Application;

private:
    explicit ApplicationPrivate(): active(true) {}

private:
    static void onContextNotify(pa_context* context, void* userData)
    {
        Q_UNUSED(context);
        Q_UNUSED(userData);

        pa_threaded_mainloop_signal(paMainLoop, 0);
    }

    static void onContextSubscription(pa_context* context, pa_subscription_event_type_t event, uint32_t idx, void* userData)
    {
        long facility = (event & PA_SUBSCRIPTION_EVENT_FACILITY_MASK);
        long eventType = (event & PA_SUBSCRIPTION_EVENT_TYPE_MASK);

        static QHash< long, QString > facilities;

        if (facilities.isEmpty())
        {
            facilities.insert(PA_SUBSCRIPTION_EVENT_SINK, "PA_SUBSCRIPTION_EVENT_SINK");
            facilities.insert(PA_SUBSCRIPTION_EVENT_SOURCE, "PA_SUBSCRIPTION_EVENT_SOURCE");
            facilities.insert(PA_SUBSCRIPTION_EVENT_SINK_INPUT, "PA_SUBSCRIPTION_EVENT_SINK_INPUT");
            facilities.insert(PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT, "PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT");
            facilities.insert(PA_SUBSCRIPTION_EVENT_MODULE, "PA_SUBSCRIPTION_EVENT_MODULE");
            facilities.insert(PA_SUBSCRIPTION_EVENT_CLIENT, "PA_SUBSCRIPTION_EVENT_CLIENT");
            facilities.insert(PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE, "PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE");
            facilities.insert(PA_SUBSCRIPTION_EVENT_CARD, "PA_SUBSCRIPTION_EVENT_CARD");
        }

        static QHash< long, QString > eventTypes;

        if (eventTypes.isEmpty())
        {
            eventTypes.insert(PA_SUBSCRIPTION_EVENT_NEW, "PA_SUBSCRIPTION_EVENT_NEW");
            eventTypes.insert(PA_SUBSCRIPTION_EVENT_CHANGE, "PA_SUBSCRIPTION_EVENT_CHANGE");
            eventTypes.insert(PA_SUBSCRIPTION_EVENT_REMOVE, "PA_SUBSCRIPTION_EVENT_REMOVE");
        }

        qDebug() << "Facility: " << facilities.value(facility, "Other") <<
                    "Event Type: " << eventTypes.value(eventType, "Other");


    }

    static void onContextSubscriptionSuccess(pa_context* context, int success, void* userData)
    {
        Q_UNUSED(context);
        Q_UNUSED(userData);

        qDebug() << "success: " << success;

        pa_threaded_mainloop_signal(ApplicationPrivate::paMainLoop, 0);
    }

private:
    bool active;

    QScopedPointer< Database > database;

    QScopedPointer< Model > model;

    static pa_threaded_mainloop* paMainLoop;
    static pa_mainloop_api* paMainLoopApi;
    static pa_context* paContext;

    QScopedPointer< QOfonoManager > qofonoManager;
    QScopedPointer< QOfonoVoiceCallManager > qofonoVoiceCallManager;

    QScopedPointer< Settings > settings;

    // stores object paths and its recorders
    QHash< QString, VoiceCallRecorder* > voiceCallRecorders;
};

pa_threaded_mainloop* Application::ApplicationPrivate::paMainLoop = NULL;
pa_mainloop_api* Application::ApplicationPrivate::paMainLoopApi = NULL;
pa_context* Application::ApplicationPrivate::paContext = NULL;

Application::Application(int argc, char* argv[])
    : QCoreApplication(argc, argv),
      d(new ApplicationPrivate())
{
    qDebug() << __PRETTY_FUNCTION__;

    setApplicationName(QLatin1String("harbour-callrecorder"));
    setOrganizationName(QLatin1String("kz.dpurgin"));

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

    initPulseAudio();

    d->database.reset(new Database());
    d->model.reset(new Model());
    d->settings.reset(new Settings());
}

Application::~Application()
{
    qDebug() << __PRETTY_FUNCTION__;

    pa_context_disconnect(ApplicationPrivate::paContext);

    pa_threaded_mainloop_wait(ApplicationPrivate::paMainLoop);

    pa_context_unref(ApplicationPrivate::paContext);

    pa_threaded_mainloop_stop(ApplicationPrivate::paMainLoop);
    pa_threaded_mainloop_free(ApplicationPrivate::paMainLoop);
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

void Application::initPulseAudio()
{
    qDebug() << __PRETTY_FUNCTION__;

    ApplicationPrivate::paMainLoop = pa_threaded_mainloop_new();
    pa_threaded_mainloop_start(ApplicationPrivate::paMainLoop);

    ApplicationPrivate::paMainLoopApi = pa_threaded_mainloop_get_api(ApplicationPrivate::paMainLoop);

    pa_threaded_mainloop_lock(ApplicationPrivate::paMainLoop);

    ApplicationPrivate::paContext = pa_context_new(ApplicationPrivate::paMainLoopApi, applicationName().toUtf8().data());

    pa_context_set_state_callback(ApplicationPrivate::paContext, &ApplicationPrivate::onContextNotify, NULL);
    pa_context_connect(ApplicationPrivate::paContext, NULL, PA_CONTEXT_NOFLAGS, NULL);

    bool done = false;
    pa_context_state_t contextState;

    while (!done)
    {
        switch (contextState = pa_context_get_state(d->paContext))
        {
            case PA_CONTEXT_UNCONNECTED: qDebug() << "Context state: PA_CONTEXT_UNCONNECTED"; break;
            case PA_CONTEXT_CONNECTING: qDebug() << "Context state: PA_CONTEXT_CONNECTING"; break;
            case PA_CONTEXT_AUTHORIZING: qDebug() << "Context state: PA_CONTEXT_AUTHORIZING"; break;
            case PA_CONTEXT_SETTING_NAME: qDebug() << "Context state: PA_CONTEXT_SETTING_NAME"; break;
            case PA_CONTEXT_READY: qDebug() << "Context state: PA_CONTEXT_READY"; done = true; break;
            case PA_CONTEXT_FAILED: qDebug() << "Context state: PA_CONTEXT_FAILED"; done = true; break;
            case PA_CONTEXT_TERMINATED: qDebug() << "Context state: PA_CONTEXT_TERMINATED"; done = true; break;
        }

        if (!done)
            pa_threaded_mainloop_wait(ApplicationPrivate::paMainLoop);
    }

    if (contextState != PA_CONTEXT_READY)
        throw CallRecorderException("Unable to connect PulseAudio context!");

    pa_context_set_subscribe_callback(ApplicationPrivate::paContext, &ApplicationPrivate::onContextSubscription, NULL);

    pa_operation* subscriptionOp = pa_context_subscribe(ApplicationPrivate::paContext,
                                                        PA_SUBSCRIPTION_MASK_ALL,
                                                        &ApplicationPrivate::onContextSubscriptionSuccess,
                                                        NULL);
    pa_threaded_mainloop_wait(ApplicationPrivate::paMainLoop);
    pa_operation_unref(subscriptionOp);


    pa_threaded_mainloop_unlock(ApplicationPrivate::paMainLoop);

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

    // ofono manager is not needed now
    d->qofonoManager->deleteLater();
}

/// Creates the recorder for a voice call appeared in the system
void Application::onVoiceCallAdded(const QString& objectPath)
{
    qDebug() << __PRETTY_FUNCTION__ << objectPath;

    if (active())
    {
        QScopedPointer< VoiceCallRecorder > voiceCallRecorder(new VoiceCallRecorder(objectPath));
        d->voiceCallRecorders.insert(objectPath, voiceCallRecorder.take());
    }
}

/// Checks whether recorder is active and cleans it up after the voice call was removed from the system
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
