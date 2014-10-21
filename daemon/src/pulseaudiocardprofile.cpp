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

#include "pulseaudiocardprofile.h"

#include <pulse/introspect.h>
#include <pulse/version.h>

class PulseAudioCardProfile::PulseAudioCardProfilePrivate
{
    friend class PulseAudioCardProfile;
    friend class QScopedPointerDeleter< PulseAudioCardProfilePrivate >;

    explicit PulseAudioCardProfilePrivate()
        : sinks(0),
          sources(0),
          priority(0)
#if PA_CHECK_VERSION(5, 0, 0)
        , available(false)
#endif
    {
    }
    ~PulseAudioCardProfilePrivate()
    {
    }

    QString name;
    QString description;

    quint32 sinks;
    quint32 sources;
    quint32 priority;

#if PA_CHECK_VERSION(5, 0, 0)
    bool available;
#endif
};

#if PA_CHECK_VERSION(5, 0, 0)
PulseAudioCardProfile::PulseAudioCardProfile(pa_card_profile_info2* paCardProfileInfo)
#else
PulseAudioCardProfile::PulseAudioCardProfile(pa_card_profile_info* paCardProfileInfo)
#endif
    : d(new PulseAudioCardProfilePrivate)
{
    d->name = QLatin1String(paCardProfileInfo->name);
    d->description = QLatin1String(paCardProfileInfo->description);

    d->sinks = paCardProfileInfo->n_sinks;
    d->sources = paCardProfileInfo->n_sources;
    d->priority = paCardProfileInfo->priority;

#if PA_CHECK_VERSION(5, 0, 0)
    d->available = paCardProfileInfo->available;
#endif
}

PulseAudioCardProfile::~PulseAudioCardProfile()
{
}

#if PA_CHECK_VERSION(5, 0, 0)
bool PulseAudioCardProfile::available() const
{
    return d->available;
}
#endif

QString PulseAudioCardProfile::description() const
{
    return d->description;
}

QString PulseAudioCardProfile::name() const
{
    return d->name;
}

quint32 PulseAudioCardProfile::priority() const
{
    return d->priority;
}

quint32 PulseAudioCardProfile::sinks() const
{
    return d->sinks;
}

quint32 PulseAudioCardProfile::sources() const
{
    return d->sources;
}
