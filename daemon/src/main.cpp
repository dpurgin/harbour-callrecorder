#include "application.h"

#include <QDebug>
#include <QScopedPointer>

#include <libcallrecorder/callrecorderexception.h>

int main(int argc, char* argv[])
{
    int retval = 0;

    try
    {
        QScopedPointer< Application > a(new Application(argc, argv));
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

    return retval;
}

