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

#ifndef HARBOUR_CALLRECORDERD_DBUSADAPTOR_H
#define HARBOUR_CALLRECORDERD_DBUSADAPTOR_H

#include <QDBusAbstractAdaptor>

class Application;

class DBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "kz.dpurgin.CallRecorder")

public:
    explicit DBusAdaptor(Application* parent);
    virtual ~DBusAdaptor();

signals:
    void RecorderStateChanged();

#ifdef QT_DEBUG
    void EmulatedLineIdentificationChanged(QString lineIdentification);
    void EmulatedVoiceCallAdded(QString objectPath);
    void EmulatedVoiceCallRemoved(QString objectPath);
    void EmulatedVoiceCallStateChanged(QString state);
#endif // QT_DEBUG

public slots:
    Q_NOREPLY void CheckStorageLimits();
    Q_NOREPLY void ShowApprovalDialog();

#ifdef QT_DEBUG
    Q_NOREPLY void EmulateLineIdentification(QString lineIdentification);
    Q_NOREPLY void EmulateVoiceCallAdded(QString objectPath);
    Q_NOREPLY void EmulateVoiceCallRemoved(QString objectPath);
    Q_NOREPLY void EmulateVoiceCallState(QString state);
    int GetPhoneNumberIdByLineIdentification(QString lineIdentification);
#endif // QT_DEBUG

private:
    class DBusAdaptorPrivate;
    DBusAdaptorPrivate* d;

};

#endif // HARBOUR_CALLRECORDERD_DBUSADAPTOR_H
