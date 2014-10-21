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

#ifndef HARBOUR_CALLRECORDERD_PULSEAUDIOCARDPROFILE_H
#define HARBOUR_CALLRECORDERD_PULSEAUDIOCARDPROFILE_H

#include <QObject>
#include <QScopedPointer>

#include <pulse/version.h>

#if PA_CHECK_VERSION(5, 0, 0)
    struct pa_card_profile_info2;
#else
    struct pa_card_profile_info;
#endif

class PulseAudioCardProfile
{
    Q_DISABLE_COPY(PulseAudioCardProfile)

public:
#if PA_CHECK_VERSION(5, 0, 0)
    explicit PulseAudioCardProfile(pa_card_profile_info2* paCardProfileInfo);
#else
    explicit PulseAudioCardProfile(pa_card_profile_info* paCardProfileInfo);
#endif
    virtual ~PulseAudioCardProfile();

#if PA_CHECK_VERSION(5, 0, 0)
    bool available() const;
#endif
    QString description() const;
    QString name() const;
    quint32 priority() const;
    quint32 sinks() const;
    quint32 sources() const;

private:
    class PulseAudioCardProfilePrivate;
    QScopedPointer< PulseAudioCardProfilePrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_PULSEAUDIOCARDPROFILE_H
