#    Call Recorder for SailfishOS
#    Copyright (C) 2014  Dmitriy Purgin <dpurgin@gmail.com>

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

include(../common.pri)

TARGET = $${PACKAGE}

CONFIG += sailfishapp
PKGCONFIG += flac

QT += core quick qml multimedia sql

INCLUDEPATH += ../libcallrecorder/include

LIBS += -L../libcallrecorder -lcallrecorder

SOURCES += \
    src/main.cpp

OTHER_FILES += \
    harbour-callrecorder.desktop \
    translations/*.ts \
    qml/pages/AboutPage.qml \
    qml/cover/CoverPage.qml \
    qml/pages/EventsPage.qml \
    qml/pages/EventPage.qml \
    qml/main.qml

# to disable building translations every time, comment out the
# following CONFIG line
#CONFIG += sailfishapp_i18n
#TRANSLATIONS += translations/ui-de.ts

