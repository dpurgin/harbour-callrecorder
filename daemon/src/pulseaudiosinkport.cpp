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


#include "pulseaudiosinkport.h"

#include <pulse/introspect.h>

class PulseAudioSinkPort::PulseAudioSinkPortPrivate
{
    friend class PulseAudioSinkPort;

    QString name;
    QString description;
    PulseAudioSinkPort::PortAvailability available;
    quint32 priority;
};

PulseAudioSinkPort::PulseAudioSinkPort(const pa_sink_port_info* portInfo)
    : d(new PulseAudioSinkPortPrivate())
{
    if (portInfo->available == PA_PORT_AVAILABLE_NO)
        d->available = NotAvailable;
    else if (portInfo->available == PA_PORT_AVAILABLE_YES)
        d->available = Available;
    else
        d->available = Unknown;

    d->name = QLatin1String(portInfo->name);
    d->description = QLatin1String(portInfo->description);
    d->priority = portInfo->priority;
}

PulseAudioSinkPort::~PulseAudioSinkPort()
{
    delete d;
}

PulseAudioSinkPort::PortAvailability PulseAudioSinkPort::available() const
{
    return d->available;
}

QString PulseAudioSinkPort::description() const
{
    return d->description;
}

QString PulseAudioSinkPort::name() const
{
    return d->name;
}

quint32 PulseAudioSinkPort::priority() const
{
    return d->priority;
}
