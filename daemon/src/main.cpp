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

#include "application.h"

#include <QDebug>
#include <QDBusConnection>
#include <QScopedPointer>

#include <libcallrecorder/callrecorderexception.h>

int main(int argc, char* argv[])
{
    int retval = 0;

    try
    {
        QScopedPointer< Application > a(new Application(argc, argv));

        if (QDBusConnection::sessionBus().registerService("kz.dpurgin.DBus.CallRecorder"))
            QDBusConnection::sessionBus().registerObject("/Daemon", a.data());
        else
            qWarning() << "Unable to register DBus service";

        retval = a->exec();
    }
    catch (CallRecorderException& e)
    {
        qCritical() << e.what();
    }
    catch (...)
    {
        qCritical() << __PRETTY_FUNCTION__ << "Unhandled exception occured";
        retval = -1;
    }

    QDBusConnection::sessionBus().unregisterService("kz.dpurgin.DBus.CallRecorder");

    return retval;
}

