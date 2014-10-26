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

#ifndef HARBOUR_CALLRECORDERD_PULSEAUDIOCARD_H
#define HARBOUR_CALLRECRODERD_PULSEAUDIOCARD_H

#include <QObject>
#include <QScopedPointer>

struct pa_context;
struct pa_card_info;

class PulseAudioCardProfile;

class PulseAudioCard : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(PulseAudioCard)    

public:
    explicit PulseAudioCard(pa_context* paContext, const pa_card_info* paCardInfo, QObject* parent = 0);
    virtual ~PulseAudioCard();

    PulseAudioCardProfile* activeProfile() const;
    void setActiveProfile(const QString& activeProfile);

    QString driver() const;
    quint32 index() const;
    QString name() const;
    quint32 ownerModule() const;

    void update(const pa_card_info* paCardInfo);

signals:
    void activeProfileChanged(const PulseAudioCardProfile* activeProfile);
    void driverChanged(const QString& driver);
    void nameChanged(const QString& name);
    void ownerModuleChanged(quint32 ownerModule);
    void propertiesChanged();

public slots:

private:
    class PulseAudioCardPrivate;
    QScopedPointer< PulseAudioCardPrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_PULSEAUDIOCARD_H
