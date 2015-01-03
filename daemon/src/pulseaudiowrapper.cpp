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
#include <QThread>

#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/mainloop-api.h>
#include <pulse/subscribe.h>
#include <pulse/thread-mainloop.h>

#include <libcallrecorder/callrecorderexception.h>

#include "application.h"
#include "pulseaudiocard.h"
#include "pulseaudiosink.h"
#include "pulseaudiosource.h"

class PulseAudioWrapper::PulseAudioWrapperPrivate
{
    friend class PulseAudioWrapper;

    static void asyncSuccessCallback(pa_context* context, int success, void* userData)
    {
        Q_UNUSED(context);
        Q_UNUSED(userData);

        qDebug() << "success: " << success;
    }

    static void onCardInfoByIndex(pa_context* context, const pa_card_info* info, int eol, void* userData)
    {
        if (!eol)
        {
            PulseAudioWrapperPrivate* d = reinterpret_cast< PulseAudioWrapperPrivate* >(userData);

            if (d->cardsByIndex.contains(info->index))
                d->cardsByIndex.value(info->index)->update(info);
            else
            {
                PulseAudioCard* card = new PulseAudioCard(context, info);

                d->cards.insert(card);
                d->cardsByIndex.insert(card->index(), card);
                d->cardsByName.insert(card->name(), card);
            }
        }
    }

    static void onCardInfoList(pa_context* context, const pa_card_info* cardInfo, int eol, void* userData)
    {
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
        long facility = (event & PA_SUBSCRIPTION_EVENT_FACILITY_MASK);
        long eventType = (event & PA_SUBSCRIPTION_EVENT_TYPE_MASK);

//        static QHash< long, QString > facilities;

//        if (facilities.isEmpty())
//        {
//            facilities.insert(PA_SUBSCRIPTION_EVENT_SINK, "PA_SUBSCRIPTION_EVENT_SINK");
//            facilities.insert(PA_SUBSCRIPTION_EVENT_SOURCE, "PA_SUBSCRIPTION_EVENT_SOURCE");
//            facilities.insert(PA_SUBSCRIPTION_EVENT_SINK_INPUT, "PA_SUBSCRIPTION_EVENT_SINK_INPUT");
//            facilities.insert(PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT, "PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT");
//            facilities.insert(PA_SUBSCRIPTION_EVENT_MODULE, "PA_SUBSCRIPTION_EVENT_MODULE");
//            facilities.insert(PA_SUBSCRIPTION_EVENT_CLIENT, "PA_SUBSCRIPTION_EVENT_CLIENT");
//            facilities.insert(PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE, "PA_SUBSCRIPTION_EVENT_SAMPLE_CACHE");
//            facilities.insert(PA_SUBSCRIPTION_EVENT_CARD, "PA_SUBSCRIPTION_EVENT_CARD");
//        }

//        static QHash< long, QString > eventTypes;

//        if (eventTypes.isEmpty())
//        {
//            eventTypes.insert(PA_SUBSCRIPTION_EVENT_NEW, "PA_SUBSCRIPTION_EVENT_NEW");
//            eventTypes.insert(PA_SUBSCRIPTION_EVENT_CHANGE, "PA_SUBSCRIPTION_EVENT_CHANGE");
//            eventTypes.insert(PA_SUBSCRIPTION_EVENT_REMOVE, "PA_SUBSCRIPTION_EVENT_REMOVE");
//        }

//        qDebug() << "Facility:" << facilities.value(facility, "Other") <<
//                    "Event Type:" << eventTypes.value(eventType, "Other") <<
//                    "idx:" << idx;

        PulseAudioWrapperPrivate* d = reinterpret_cast< PulseAudioWrapperPrivate* >(userData);

        if (facility == PA_SUBSCRIPTION_EVENT_CARD)
        {
            if (eventType == PA_SUBSCRIPTION_EVENT_NEW ||
                    eventType == PA_SUBSCRIPTION_EVENT_CHANGE)
            {
                pa_operation_unref(pa_context_get_card_info_by_index(
                                       PulseAudioWrapperPrivate::paContext,
                                       idx,
                                       &PulseAudioWrapperPrivate::onCardInfoByIndex,
                                       userData));
            }
            else if (eventType == PA_SUBSCRIPTION_EVENT_REMOVE)
            {
                qDebug() << "Removing card at idx: " << idx;

                if (d->cardsByIndex.contains(idx))
                {
                    PulseAudioCard* card = d->cardsByIndex.value(idx);

                    d->cards.remove(card);
                    d->cardsByIndex.remove(idx);
                    d->cardsByName.remove(card->name());

                    delete card;
                }
                else
                    qDebug() << "No card at idx " << idx;
            }
        }
        else if (facility == PA_SUBSCRIPTION_EVENT_SINK)
        {
            if (eventType == PA_SUBSCRIPTION_EVENT_NEW ||
                    eventType == PA_SUBSCRIPTION_EVENT_CHANGE)
            {
                pa_operation_unref(pa_context_get_sink_info_by_index(
                                       PulseAudioWrapperPrivate::paContext,
                                       idx,
                                       &PulseAudioWrapperPrivate::onSinkInfoByIndex,
                                       userData));
            }
            else if (eventType == PA_SUBSCRIPTION_EVENT_REMOVE)
            {
                qDebug() << "Removing sink at idx: " << idx;

                if (d->sinksByIndex.contains(idx))
                {
                    PulseAudioSink* sink = d->sinksByIndex.value(idx);

                    d->sinks.remove(sink);
                    d->sinksByIndex.remove(idx);
                    d->sinksByName.remove(sink->name());

                    delete sink;
                }
                else
                    qDebug() << "No sink at idx " << idx;
            }
        }
        else if (facility == PA_SUBSCRIPTION_EVENT_SOURCE)
        {
            if (eventType == PA_SUBSCRIPTION_EVENT_NEW)
            {
                pa_operation_unref(pa_context_get_source_info_by_index(
                                       PulseAudioWrapperPrivate::paContext,
                                       idx,
                                       &PulseAudioWrapperPrivate::onSourceInfoByIndex,
                                       userData));
            }
            else if (eventType == PA_SUBSCRIPTION_EVENT_REMOVE)
            {
                qDebug() << "Removing source at idx: " << idx;

                if (d->sourcesByIndex.contains(idx))
                {
                    PulseAudioSource* source = d->sourcesByIndex.value(idx);

                    d->sources.remove(source);
                    d->sourcesByIndex.remove(idx);
                    d->sourcesByName.remove(source->name());

                    delete source;
                }
                else
                    qDebug() << "No source at idx " << idx;
            }
        }
    }

    static void onContextSubscriptionSuccess(pa_context* context, int success, void* userData)
    {
        Q_UNUSED(context);
        Q_UNUSED(userData);

        qDebug() << "success: " << success;

        pa_threaded_mainloop_signal(PulseAudioWrapperPrivate::paMainLoop, 0);
    }

    static void onSinkInfoByIndex(pa_context* context, const pa_sink_info* sinkInfo, int eol, void* userData)
    {        
        if (!eol)
        {
            PulseAudioWrapperPrivate* d = reinterpret_cast< PulseAudioWrapperPrivate* >(userData);

            if (d->sinksByIndex.contains(sinkInfo->index))
                d->sinksByIndex.value(sinkInfo->index)->update(sinkInfo);
            else
            {
                PulseAudioSink* sink = new PulseAudioSink(context, sinkInfo);

                d->sinks.insert(sink);
                d->sinksByIndex.insert(sink->index(), sink);
                d->sinksByName.insert(sink->name(), sink);
            }
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

    static void onSourceInfoByIndex(pa_context* context, const pa_source_info* sourceInfo, int eol, void* userData)
    {
        if (!eol)
        {
            PulseAudioWrapperPrivate* d = reinterpret_cast< PulseAudioWrapperPrivate* >(userData);

            if (!d->sourcesByIndex.contains(sourceInfo->index))
            {
                PulseAudioSource* source = new PulseAudioSource(context, sourceInfo);

                d->sources.insert(source);
                d->sourcesByIndex.insert(source->index(), source);
                d->sourcesByName.insert(source->name(), source);
            }
        }
    }

    static void onSourceInfoList(pa_context* context, const pa_source_info* sourceInfo, int eol, void* userData)
    {
        if (eol)
            pa_threaded_mainloop_signal(PulseAudioWrapperPrivate::paMainLoop, 0);
        else
        {
            PulseAudioWrapperPrivate* d = reinterpret_cast< PulseAudioWrapperPrivate* >(userData);

            PulseAudioSource* source = new PulseAudioSource(context, sourceInfo);

            d->sources.insert(source);
            d->sourcesByIndex.insert(source->index(), source);
            d->sourcesByName.insert(source->name(), source);
        }
    }

    static pa_threaded_mainloop* paMainLoop;
    static pa_mainloop_api* paMainLoopApi;
    static pa_context* paContext;

    QSet< PulseAudioCard* > cards; // this one owns the pointers
    QHash< quint32, PulseAudioCard* > cardsByIndex; // indices in pulseaudio can be somehow sparse
    QHash< QString, PulseAudioCard* > cardsByName;

    QSet< PulseAudioSink* > sinks; // owns the pointers
    QHash< quint32, PulseAudioSink* > sinksByIndex; // see cardsByIndex above
    QHash< QString, PulseAudioSink* > sinksByName;

    QSet< PulseAudioSource* > sources; // owns the pointers
    QHash< quint32, PulseAudioSource* > sourcesByIndex; // see cardsByIndex above
    QHash< QString, PulseAudioSource* > sourcesByName;
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

    pa_operation* listSourcesOp = pa_context_get_source_info_list(PulseAudioWrapperPrivate::paContext,
                                                                  &PulseAudioWrapperPrivate::onSourceInfoList,
                                                                  d.data());
    pa_threaded_mainloop_wait(PulseAudioWrapperPrivate::paMainLoop);
    pa_operation_unref(listSourcesOp);

    pa_context_set_subscribe_callback(PulseAudioWrapperPrivate::paContext,
                                      &PulseAudioWrapperPrivate::onContextSubscription,
                                      d.data());

    pa_operation* subscriptionOp = pa_context_subscribe(PulseAudioWrapperPrivate::paContext,
                                                        static_cast< pa_subscription_mask_t >(
                                                            PA_SUBSCRIPTION_MASK_CARD |
                                                            PA_SUBSCRIPTION_MASK_SINK |
                                                            PA_SUBSCRIPTION_MASK_SOURCE),
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

void PulseAudioWrapper::setDefaultSource(const QString& sourceName)
{
    qDebug() << sourceName;

    pa_operation_unref(pa_context_set_default_source(PulseAudioWrapperPrivate::paContext,
                                                     sourceName.toUtf8().data(),
                                                     PulseAudioWrapperPrivate::asyncSuccessCallback,
                                                     NULL));
}

PulseAudioSink* PulseAudioWrapper::sinkByIndex(quint32 index) const
{
    return d->sinksByIndex.size() > index? d->sinksByIndex[index]: NULL;
}

PulseAudioSink* PulseAudioWrapper::sinkByName(const QString& name) const
{
    return d->sinksByName.value(name, NULL);
}
