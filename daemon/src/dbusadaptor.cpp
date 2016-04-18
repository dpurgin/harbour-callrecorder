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

#include "dbusadaptor.h"

#include "application.h"

#ifndef Q_NO_DEBUG
#include "model.h"

#include <libcallrecorder/phonenumberstablemodel.h>
#endif

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

void DBusAdaptor::ShowApprovalDialog()
{
    d->application->showApprovalDialog();
}

#ifndef Q_NO_DEBUG
int DBusAdaptor::GetPhoneNumberIdByLineIdentification(QString lineIdentification)
{
    return d->application->model()->phoneNumbers()->getIdByLineIdentification(lineIdentification);
}
#endif
