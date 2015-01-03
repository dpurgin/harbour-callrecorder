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

#ifndef HARBOUR_CALLRECORDERD_PULSEAUDIOWRAPPER_H
#define HARBOUR_CALLRECORDERD_PULSEAUDIOWRAPPER_H

#include <QObject>
#include <QScopedPointer>

class PulseAudioCard;
class PulseAudioSink;

class PulseAudioWrapper : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PulseAudioWrapper)

public:
    explicit PulseAudioWrapper(QObject *parent = 0);
    virtual ~PulseAudioWrapper();

    PulseAudioCard* cardByIndex(quint32 index) const;
    PulseAudioCard* cardByName(const QString& name) const;

    PulseAudioSink* sinkByIndex(quint32 index) const;
    PulseAudioSink* sinkByName(const QString& name) const;

    void setDefaultSource(const QString& sourceName);

signals:

public slots:

private:
    class PulseAudioWrapperPrivate;
    QScopedPointer< PulseAudioWrapperPrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_PULSEAUDIOWRAPPER_H
