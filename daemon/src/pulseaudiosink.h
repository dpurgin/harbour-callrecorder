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

#ifndef HARBOUR_CALLRECORDERD_PULSEAUDIOSINK_H
#define HARBOUR_CALLRECORDERD_PULSEAUDIOSINK_H

#include <QObject>

class PulseAudioSinkPort;

struct pa_sink_info;

class PulseAudioSink : public QObject
{
    Q_OBJECT
public:
    explicit PulseAudioSink(const pa_sink_info* sinkInfo, QObject *parent = 0);
    virtual ~PulseAudioSink();

    PulseAudioSinkPort* activePort() const;
    quint32 index() const;
    QString name() const;

signals:

public slots:

private:
    class PulseAudioSinkPrivate;
    PulseAudioSinkPrivate* d;
};

#endif // HARBOUR_CALLRECORDERD_PULSEAUDIOSINK_H
