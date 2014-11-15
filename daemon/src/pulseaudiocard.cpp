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

#include "pulseaudiocard.h"

#include "pulseaudiocardprofile.h"

#include <QDebug>
#include <QHash>
#include <QSet>
#include <QThread>
#include <QVariant>

#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/version.h>

class PulseAudioCard::PulseAudioCardPrivate
{
    friend class PulseAudioCard;
    friend class QScopedPointerDeleter< PulseAudioCardPrivate >;

    PulseAudioCardPrivate()
        : activeProfile(NULL),
          context(NULL)
    {
    }
    ~PulseAudioCardPrivate()
    {
    }

    static void trivialCallback(pa_context*, int, void*)
    {
    }

    pa_context* context;

    PulseAudioCardProfile* activeProfile;
    QString driver;
    quint32 index;
    QString name;
    quint32 ownerModule;
    QSet< PulseAudioCardProfile* > profiles;  // this one owns the pointers
    QHash< QString, PulseAudioCardProfile* > profilesByName;
    QHash< QString, QVariant> properties;
};

PulseAudioCard::PulseAudioCard(pa_context* context, const pa_card_info* paCardInfo, QObject *parent)
    : QObject(parent),
      d(new PulseAudioCardPrivate)
{
    qDebug() << QThread::currentThread();
    qDebug() << "Discovered card: " << paCardInfo->name << ", index: " << paCardInfo->index;

    d->context = context;

    d->index = paCardInfo->index;
    d->name = QLatin1String(paCardInfo->name);
    d->ownerModule = paCardInfo->owner_module;
    d->driver = QLatin1String(paCardInfo->driver);

    for (quint32 i = 0; i < paCardInfo->n_profiles; i++)
    {
#if PA_CHECK_VERSION(5, 0, 0)
        PulseAudioCardProfile* profile = new PulseAudioCardProfile(paCardInfo->profiles2[i]);
#else
        PulseAudioCardProfile* profile = new PulseAudioCardProfile(&paCardInfo->profiles[i]);
#endif

        d->profiles.insert(profile);
        d->profilesByName.insert(profile->name(), profile);
    }

    d->activeProfile = d->profilesByName.value(paCardInfo->active_profile->name);
}

PulseAudioCard::~PulseAudioCard()
{
    qDeleteAll(d->profiles);
}

PulseAudioCardProfile* PulseAudioCard::activeProfile() const
{
    return d->activeProfile;
}

QString PulseAudioCard::driver() const
{
    return d->driver;
}

quint32 PulseAudioCard::index() const
{
    return d->index;
}

QString PulseAudioCard::name() const
{
    return d->name;
}

quint32 PulseAudioCard::ownerModule() const
{
    return d->ownerModule;
}

void PulseAudioCard::update(const pa_card_info* paCardInfo)
{
    QSet< QString > emitters;

    if (d->name != QLatin1String(paCardInfo->name))
    {
        d->name = QLatin1String(paCardInfo->name);
        emitters.insert(QLatin1String("name"));
    }

    if (d->ownerModule != paCardInfo->owner_module)
    {
        d->ownerModule = paCardInfo->owner_module;
        emitters.insert(QLatin1String("ownerModule"));
    }

    if (d->driver != QLatin1String(paCardInfo->driver))
    {
        d->driver = QLatin1String(paCardInfo->driver);
        emitters.insert(QLatin1String("driver"));
    }

    // for now, assume that profiles can never be added/removed/changed at runtime

    if (d->activeProfile->name() != QLatin1String(paCardInfo->active_profile->name))
    {
        d->activeProfile = d->profilesByName.value(paCardInfo->active_profile->name, NULL);
        emitters.insert(QLatin1String("activeProfile"));
    }

    if (emitters.size() > 0)
    {
        emit propertiesChanged();

        foreach (QString emitter, emitters)
        {
            if (emitter == QLatin1String("name"))
                emit nameChanged(d->name);
            else if (emitter == QLatin1String("ownerModule"))
                emit ownerModuleChanged(d->ownerModule);
            else if (emitter == QLatin1String("driver"))
                emit driverChanged(d->driver);
            else if (emitter == QLatin1String("activeProfile"))
                emit activeProfileChanged(d->activeProfile);
        }
    }

}

void PulseAudioCard::setActiveProfile(const QString& profileName)
{
    if (d->activeProfile && profileName == d->activeProfile->name())
        return;

    pa_operation_unref(pa_context_set_card_profile_by_index(d->context,
                                                            index(),
                                                            profileName.toUtf8().data(),
                                                            &PulseAudioCardPrivate::trivialCallback,
                                                            NULL));
}
