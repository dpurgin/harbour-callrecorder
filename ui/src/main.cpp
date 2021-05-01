/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2016 Dmitriy Purgin <dpurgin@gmail.com>

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

#include <sailfishapp.h>

#include <QGuiApplication>
#include <QtQuick>
#include <QScopedPointer>

#include <exception>

#include <libcallrecorder/blacklisttablemodel.h>
#include <libcallrecorder/callrecorderexception.h>
#include <libcallrecorder/database.h>
#include <libcallrecorder/eventstablemodel.h>
#include <libcallrecorder/libcallrecorder.h>
#include <libcallrecorder/phonenumberstablemodel.h>
#include <libcallrecorder/settings.h>
#include <libcallrecorder/whitelisttablemodel.h>

#include "backuphelper.h"
#include "filesystemhelper.h"
#include "localelistmodel.h"

int main(int argc, char *argv[])
{
    LibCallRecorder::installMessageHandler();

    qmlRegisterType< LocaleListModel >(
                "kz.dpurgin.callrecorder.LocaleListModel", 1, 0, "LocaleListModel");
    qmlRegisterType< Settings >("kz.dpurgin.callrecorder.Settings", 1, 0, "Settings");

    int retval = -1;

    try
    {
        QScopedPointer< QGuiApplication > app(SailfishApp::application(argc, argv));

        app->setOrganizationName("kz.dpurgin");
        app->setApplicationName("harbour-callrecorder");

        QScopedPointer< QTranslator > uiTranslator(LibCallRecorder::createTranslator("ui"));
        app->installTranslator(uiTranslator.data());

        QScopedPointer< Database > db(new Database());

        QScopedPointer< EventsTableModel > eventsModel(new EventsTableModel(db.data()));
        QScopedPointer< BlackListTableModel > blackListModel(new BlackListTableModel(db.data()));
        QScopedPointer< PhoneNumbersTableModel > phoneNumbersModel(
                    new PhoneNumbersTableModel(db.data()));
        QScopedPointer< WhiteListTableModel > whiteListModel(new WhiteListTableModel(db.data()));

        QScopedPointer< FileSystemHelper > fileSystemHelper(new FileSystemHelper());

    //    QScopedPointer< Settings > settings(new Settings());

        QScopedPointer< QQuickView > view(SailfishApp::createView());

        view->engine()->addImportPath("/usr/share/harbour-callrecorder/lib/imports");

        view->setSource(SailfishApp::pathTo("qml/main.qml"));
        view->show();

        view->rootContext()->setContextProperty("eventsModel", eventsModel.data());
        view->rootContext()->setContextProperty("blackListModel", blackListModel.data());
        view->rootContext()->setContextProperty("phoneNumbersModel", phoneNumbersModel.data());
        view->rootContext()->setContextProperty("whiteListModel", whiteListModel.data());

        view->rootContext()->setContextProperty("fileSystemHelper", fileSystemHelper.data());

        view->rootContext()->setContextProperty("license",
                                                "Call Recorder for SailfishOS"
                                                "\nCopyright (C) 2014-2021 Dmitriy Purgin <dpurgin@gmail.com>"
                                                "\nhttps://github.com/dpurgin/harbour-callrecorder"
                                                "\n"
                                                "\nThis program is free software: you can redistribute it and/or modify"
                                                " it under the terms of the GNU General Public License as published by"
                                                " the Free Software Foundation, either version 3 of the License, or"
                                                " (at your option) any later version."
                                                "\n"
                                                "\nThis program is distributed in the hope that it will be useful,"
                                                " but WITHOUT ANY WARRANTY; without even the implied warranty of"
                                                " MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
                                                " GNU General Public License for more details."
                                                "\n"
                                                "\nYou should have received a copy of the GNU General Public License"
                                                " along with this program.  If not, see <http://www.gnu.org/licenses/>.");

        view->rootContext()->setContextProperty("VERSION", QLatin1String(VERSION));

    //    view->rootContext()->setContextProperty("settings", settings.data());

        retval = app->exec();
    }
    catch (CallRecorderException& e)
    {
        qCritical() << "Exception occured: " << e.qWhat();
    }
    catch (std::exception& e)
    {
        qCritical() << "Exception occured: " << e.what();
    }
    catch (...)
    {
        qCritical() << "Unhandled exception occured";
    }

    return retval;
}

