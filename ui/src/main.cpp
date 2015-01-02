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

#include <sailfishapp.h>

#include <QGuiApplication>
#include <QtQuick>
#include <QScopedPointer>

#include <libcallrecorder/database.h>
#include <libcallrecorder/eventstablemodel.h>
#include <libcallrecorder/settings.h>

#include "filesystemhelper.h"

int main(int argc, char *argv[])
{
    qmlRegisterType< Settings >("kz.dpurgin.callrecorder.Settings", 1, 0, "Settings");

    QScopedPointer< QGuiApplication > app(SailfishApp::application(argc, argv));

    app->setOrganizationName("kz.dpurgin");
    app->setApplicationName("harbour-callrecorder");

    QScopedPointer< Database > db(new Database());

    QScopedPointer< EventsTableModel > eventsModel(new EventsTableModel(db.data()));

    QScopedPointer< FileSystemHelper > fileSystemHelper(new FileSystemHelper());

//    QScopedPointer< Settings > settings(new Settings());

    QScopedPointer< QQuickView > view(SailfishApp::createView());
    view->setSource(SailfishApp::pathTo("qml/main.qml"));
    view->show();

    view->rootContext()->setContextProperty("eventsModel", eventsModel.data());
    view->rootContext()->setContextProperty("fileSystemHelper", fileSystemHelper.data());

    view->rootContext()->setContextProperty("license",
                                            "Call Recorder for SailfishOS"
                                            "\nCopyright (C) 2014  Dmitriy Purgin <dpurgin@gmail.com>"
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

//    view->rootContext()->setContextProperty("settings", settings.data());

    return app->exec();
}

