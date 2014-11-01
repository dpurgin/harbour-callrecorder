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

#include "pulseaudiosink.h"

#include <QDebug>
#include <QHash>
#include <QSet>
#include <QVector>

#include <pulse/context.h>
#include <pulse/introspect.h>

#include "pulseaudiosinkport.h"

class PulseAudioSink::PulseAudioSinkPrivate
{
    friend class PulseAudioSink;

    PulseAudioSinkPrivate(): activePort(NULL), context(NULL) {}

    static void trivialCallback(pa_context*, int, void*)
    {
    }

    pa_context* context;

    QString name;
    int index;

    PulseAudioSinkPort* activePort;

    QSet< PulseAudioSinkPort* > sinkPorts;
    QHash< QString, PulseAudioSinkPort* > sinkPortsByName;
};

PulseAudioSink::PulseAudioSink(pa_context* context, const pa_sink_info* sinkInfo, QObject *parent)
    : QObject(parent),
      d(new PulseAudioSinkPrivate())
{
    qDebug() << "Discovered sink: " << sinkInfo->name << ", index: " << sinkInfo->index;

    d->context = context;

    d->name = QLatin1String(sinkInfo->name);
    d->index = sinkInfo->index;

    for (quint32 c = 0; c < sinkInfo->n_ports; c++)
    {
        PulseAudioSinkPort* sinkPort = new PulseAudioSinkPort(sinkInfo->ports[c]);

        d->sinkPorts.insert(sinkPort);
        d->sinkPortsByName.insert(sinkPort->name(), sinkPort);
    }

    if (sinkInfo->active_port)
        d->activePort = d->sinkPortsByName.value(QLatin1String(sinkInfo->active_port->name), NULL);
}

PulseAudioSink::~PulseAudioSink()
{
    qDeleteAll(d->sinkPorts);

    delete d;
}

PulseAudioSinkPort* PulseAudioSink::activePort() const
{
    return d->activePort;
}

quint32 PulseAudioSink::index() const
{
    return d->index;
}

QString PulseAudioSink::name() const
{
    return d->name;
}

void PulseAudioSink::setActivePort(const QString& port)
{
    qDebug() << "";

    if (d->activePort && d->activePort->name() != port)
    {
        pa_operation_unref(pa_context_set_sink_port_by_index(d->context,
                                                             d->index,
                                                             port.toUtf8().data(),
                                                             &PulseAudioSinkPrivate::trivialCallback,
                                                             NULL));
    }
}

void PulseAudioSink::update(const pa_sink_info* sinkInfo)
{
    QSet< QString > emitters;

    if (sinkInfo->active_port && d->activePort &&
            d->activePort->name() != QLatin1String(sinkInfo->active_port->name))
    {
        d->activePort = d->sinkPortsByName.value(sinkInfo->active_port->name, NULL);

        if (!d->activePort)
            qWarning() << "Port is active but was not discovered:" << sinkInfo->active_port->name;

        emitters.insert("activePort");
    }

    foreach (QString emitter, emitters)
    {
        if (emitter == QLatin1String("activePort"))
            emit activePortChanged(d->activePort);
    }
}
