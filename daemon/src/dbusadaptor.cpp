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

#include <QtGlobal>

#include "dbusadaptor.h"

#include "application.h"

#ifdef QT_DEBUG
#include <QDebug>

#include <libcallrecorder/phonenumberstablemodel.h>

#include "model.h"
#endif // QT_DEBUG

class DBusAdaptor::DBusAdaptorPrivate
{
    friend class DBusAdaptor;

    Application* application;
};

DBusAdaptor::DBusAdaptor(Application* parent)
    : QDBusAbstractAdaptor(parent),
      d(new DBusAdaptorPrivate)
{
    d->application = parent;
}

DBusAdaptor::~DBusAdaptor()
{
    delete d;
}

void DBusAdaptor::CheckStorageLimits()
{
    d->application->checkStorageLimits();
}

#ifdef QT_DEBUG
void DBusAdaptor::EmulateLineIdentification(QString lineIdentification)
{
    qDebug() << lineIdentification;

    emit EmulatedLineIdentificationChanged(lineIdentification);
}
#endif // QT_DEBUG

#ifdef QT_DEBUG
void DBusAdaptor::EmulateVoiceCallAdded(QString objectPath)
{
    qDebug() << objectPath;

    emit EmulatedVoiceCallAdded(objectPath);
}
#endif // QT_DEBUG

#ifdef QT_DEBUG
void DBusAdaptor::EmulateVoiceCallRemoved(QString objectPath)
{
    qDebug() << objectPath;

    emit EmulatedVoiceCallRemoved(objectPath);
}
#endif // QT_DEBUG

#ifdef QT_DEBUG
void DBusAdaptor::EmulateVoiceCallState(QString state)
{
    qDebug() << state;

    emit EmulatedVoiceCallStateChanged(state);
}
#endif // QT_DEBUG

#ifdef QT_DEBUG
int DBusAdaptor::GetPhoneNumberIdByLineIdentification(QString lineIdentification)
{
    return d->application->model()->phoneNumbers()->getIdByLineIdentification(lineIdentification);
}
#endif // QT_DEBUG

void DBusAdaptor::ShowApprovalDialog()
{
    d->application->showApprovalDialog();
}

