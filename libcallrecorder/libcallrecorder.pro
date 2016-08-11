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

TEMPLATE = lib
CONFIG += plugin

TARGET = callrecorder

target.path = /usr/share/$${PACKAGE}/lib

INSTALLS += target

QT += core sql multimedia

DEFINES += LIBCALLRECORDER

QMAKE_CXXFLAGS += -std=c++11 -Wall -Werror

SOURCES += \
    src/blacklisttablemodel.cpp \
    src/callrecorderexception.cpp \
    src/corruptfileslistmodel.cpp \
    src/database.cpp \
    src/eventstablemodel.cpp \
    src/libcallrecorder.cpp \
    src/phonenumberstablemodel.cpp \
    src/phonenumberslisttablemodel.cpp \
    src/settings.cpp \
    src/sqlcursor.cpp \
    src/whitelisttablemodel.cpp

HEADERS += \
    include/libcallrecorder/blacklisttablemodel.h \
    include/libcallrecorder/callrecorderexception.h \
    include/libcallrecorder/corruptfileslistmodel.h \
    include/libcallrecorder/database.h \
    include/libcallrecorder/eventstablemodel.h \
    include/libcallrecorder/libcallrecorder.h \
    include/libcallrecorder/phonenumberstablemodel.h \
    include/libcallrecorder/settings.h \
    include/libcallrecorder/sqlcursor.h \
    include/libcallrecorder/whitelisttablemodel.h \
    src/blacklisttablemodel.h \
    src/config.h \
    src/callrecorderexception.h \
    src/corruptfileslistmodel.h \
    src/database.h \
    src/eventstablemodel.h \
    src/libcallrecorder.h \
    src/phonenumberslisttablemodel.h \
    src/phonenumberstablemodel.h \
    src/settings.h \
    src/sqlcursor.h \
    src/whitelisttablemodel.h

RESOURCES += \
    qrc/resource.qrc

