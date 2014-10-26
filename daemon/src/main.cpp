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

