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

#include "pulseaudiowrapper.h"

#include <QDebug>
#include <QHash>
#include <QVector>

#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/mainloop-api.h>
#include <pulse/subscribe.h>
#include <pulse/thread-mainloop.h>

#include <libcallrecorder/callrecorderexception.h>

#include "application.h"
#include "pulseaudiocard.h"
#include "pulseaudiosink.h"

class PulseAudioWrapper::PulseAudioWrapperPrivate
{
    friend class PulseAudioWrapper;

    static void onCardInfoByIndex(pa_context* context, const pa_card_info* info, int eol, void* userData)
    {
        qDebug() << "entering";

        if (!eol)
        {
            PulseAudioWrapperPrivate* d = reinterpret_cast< PulseAudioWrapperPrivate* >(userData);

            d->cardsByIndex.value(info->index)->update(info);
        }
    }

    static void onCardInfoList(pa_context* context, const pa_card_info* cardInfo, int eol, void* userData)
    {
        qDebug() << "entering";

        if (eol)
            pa_threaded_mainloop_signal(PulseAudioWrapperPrivate::paMainLoop, 0);
        else
        {
            PulseAudioWrapperPrivate* d = reinterpret_cast< PulseAudioWrapperPrivate* >(userData);

            PulseAudioCard* card = new PulseAudioCard(context, cardInfo);

            d->cards.insert(card);
            d->cardsByIndex.insert(card->index(), card);
            d->cardsByName.insert(card->name(), card);
        }
    }

    static void onContextNotify(pa_context* context, void* userData)
    {
        qDebug() << "entering";

        Q_UNUSED(context);
        Q_UNUSED(userData);

        pa_threaded_mainloop_signal(paMainLoop, 0);
    }

    static void onContextSubscription(pa_context* context, pa_subscription_event_type_t event, uint32_t idx, void* userData)
    {        
        qDebug() << "entering";

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

        qDebug() << "Facility:" << facilities.value(facility, "Other") <<
                    "Event Type:" << eventTypes.value(eventType, "Other") <<
                    "idx:" << idx;

        if (facility == PA_SUBSCRIPTION_EVENT_CARD)
        {
            if (eventType == PA_SUBSCRIPTION_EVENT_CHANGE)
            {
                pa_operation_unref(pa_context_get_card_info_by_index(
                                       PulseAudioWrapperPrivate::paContext,
                                       0,
                                       &PulseAudioWrapperPrivate::onCardInfoByIndex,
                                       userData));
            }
        }
        else if (facility == PA_SUBSCRIPTION_EVENT_SINK)
        {
            if (eventType == PA_SUBSCRIPTION_EVENT_CHANGE)
            {
                pa_operation_unref(pa_context_get_sink_info_by_index(
                                       PulseAudioWrapperPrivate::paContext,
                                       idx,
                                       &PulseAudioWrapperPrivate::onSinkInfoByIndex,
                                       userData));
            }
        }
    }

    static void onContextSubscriptionSuccess(pa_context* context, int success, void* userData)
    {
        qDebug() << __PRETTY_FUNCTION__;

        Q_UNUSED(context);
        Q_UNUSED(userData);

        qDebug() << "success: " << success;

        pa_threaded_mainloop_signal(PulseAudioWrapperPrivate::paMainLoop, 0);
    }

    static void onSinkInfoByIndex(pa_context* context, const pa_sink_info* sinkInfo, int eol, void* userData)
    {        
        qDebug() << "entering";

        if (!eol)
        {
            PulseAudioWrapperPrivate* d = reinterpret_cast< PulseAudioWrapperPrivate* >(userData);

            PulseAudioSink* sink = d->sinksByName.value(QLatin1String(sinkInfo->name), NULL);

            if (sink)
                sink->update(sinkInfo);
            else
                qDebug() << "Sink was not discovered:" << sink->name();
        }
    }

    static void onSinkInfoList(pa_context* context, const pa_sink_info* sinkInfo, int eol, void* userData)
    {
        if (eol)
            pa_threaded_mainloop_signal(PulseAudioWrapperPrivate::paMainLoop, 0);
        else
        {
            PulseAudioWrapperPrivate* d = reinterpret_cast< PulseAudioWrapperPrivate* >(userData);

            PulseAudioSink* sink = new PulseAudioSink(context, sinkInfo);

            d->sinks.insert(sink);
            d->sinksByIndex.insert(sink->index(), sink);
            d->sinksByName.insert(sink->name(), sink);
        }
    }

    static pa_threaded_mainloop* paMainLoop;
    static pa_mainloop_api* paMainLoopApi;
    static pa_context* paContext;

    QSet< PulseAudioCard* > cards; // this one owns the pointers
    QVector< PulseAudioCard* > cardsByIndex;
    QHash< QString, PulseAudioCard* > cardsByName;

    QSet< PulseAudioSink* > sinks; // owns the pointers
    QVector< PulseAudioSink* > sinksByIndex;
    QHash< QString, PulseAudioSink* > sinksByName;
};

pa_threaded_mainloop* PulseAudioWrapper::PulseAudioWrapperPrivate::paMainLoop = NULL;
pa_mainloop_api* PulseAudioWrapper::PulseAudioWrapperPrivate::paMainLoopApi = NULL;
pa_context* PulseAudioWrapper::PulseAudioWrapperPrivate::paContext = NULL;

PulseAudioWrapper::PulseAudioWrapper(QObject *parent)
    : QObject(parent),
      d(new PulseAudioWrapperPrivate)
{
    PulseAudioWrapperPrivate::paMainLoop = pa_threaded_mainloop_new();
    pa_threaded_mainloop_start(PulseAudioWrapperPrivate::paMainLoop);

    PulseAudioWrapperPrivate::paMainLoopApi = pa_threaded_mainloop_get_api(PulseAudioWrapperPrivate::paMainLoop);

    pa_threaded_mainloop_lock(PulseAudioWrapperPrivate::paMainLoop);

    PulseAudioWrapperPrivate::paContext = pa_context_new(PulseAudioWrapperPrivate::paMainLoopApi,
                                                         qApp->applicationName().toUtf8().data());

    pa_context_set_state_callback(PulseAudioWrapperPrivate::paContext, &PulseAudioWrapperPrivate::onContextNotify, NULL);
    pa_context_connect(PulseAudioWrapperPrivate::paContext, NULL, PA_CONTEXT_NOFLAGS, NULL);

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
            pa_threaded_mainloop_wait(PulseAudioWrapperPrivate::paMainLoop);
    }

    if (contextState != PA_CONTEXT_READY)
        throw CallRecorderException("Unable to connect PulseAudio context!");

    pa_operation* listCardsOp = pa_context_get_card_info_list(PulseAudioWrapperPrivate::paContext,
                                                         &PulseAudioWrapperPrivate::onCardInfoList,
                                                         d.data());
    pa_threaded_mainloop_wait(PulseAudioWrapperPrivate::paMainLoop);
    pa_operation_unref(listCardsOp);

    pa_operation* listSinksOp = pa_context_get_sink_info_list(PulseAudioWrapperPrivate::paContext,
                                                             &PulseAudioWrapperPrivate::onSinkInfoList,
                                                             d.data());
    pa_threaded_mainloop_wait(PulseAudioWrapperPrivate::paMainLoop);
    pa_operation_unref(listSinksOp);

    pa_context_set_subscribe_callback(PulseAudioWrapperPrivate::paContext,
                                      &PulseAudioWrapperPrivate::onContextSubscription,
                                      d.data());

    pa_operation* subscriptionOp = pa_context_subscribe(PulseAudioWrapperPrivate::paContext,
                                                        static_cast< pa_subscription_mask_t >(
                                                            PA_SUBSCRIPTION_MASK_CARD | PA_SUBSCRIPTION_MASK_SINK),
                                                        &PulseAudioWrapperPrivate::onContextSubscriptionSuccess,
                                                        d.data());
    pa_threaded_mainloop_wait(PulseAudioWrapperPrivate::paMainLoop);
    pa_operation_unref(subscriptionOp);


    pa_threaded_mainloop_unlock(PulseAudioWrapperPrivate::paMainLoop);
}

PulseAudioWrapper::~PulseAudioWrapper()
{
    qDeleteAll(d->cards);
    qDeleteAll(d->sinks);

    pa_context_disconnect(PulseAudioWrapperPrivate::paContext);

    pa_threaded_mainloop_wait(PulseAudioWrapperPrivate::paMainLoop);

    pa_context_unref(PulseAudioWrapperPrivate::paContext);

    pa_threaded_mainloop_stop(PulseAudioWrapperPrivate::paMainLoop);
    pa_threaded_mainloop_free(PulseAudioWrapperPrivate::paMainLoop);
}

PulseAudioCard* PulseAudioWrapper::cardByIndex(quint32 index) const
{
    return d->cardsByIndex.size() > index? d->cardsByIndex[index]: NULL;
}

PulseAudioCard* PulseAudioWrapper::cardByName(const QString& name) const
{
    return d->cardsByName.value(name, NULL);
}

PulseAudioSink* PulseAudioWrapper::sinkByIndex(quint32 index) const
{
    return d->sinksByIndex.size() > index? d->sinksByIndex[index]: NULL;
}

PulseAudioSink* PulseAudioWrapper::sinkByName(const QString& name) const
{
    return d->sinksByName.value(name, NULL);
}
