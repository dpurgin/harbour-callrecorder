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

#ifndef HARBOUR_CALLRECORDERD_PULSEAUDIOSOURCE_H
#define HARBOUR_CALLRECORDERD_PULSEAUDIOSOURCE_H

#include <QObject>

struct pa_context;
struct pa_source_info;

class PulseAudioSource : public QObject
{
    Q_OBJECT

public:
    explicit PulseAudioSource(pa_context* context, const pa_source_info* info, QObject *parent = 0);
    virtual ~PulseAudioSource();

    quint32 index() const;
    QString name() const;

signals:

public slots:

private:
    class PulseAudioSourcePrivate;
    PulseAudioSourcePrivate* d;
};

#endif // HARBOUR_CALLRECORDERD_PULSEAUDIOSOURCE_H
