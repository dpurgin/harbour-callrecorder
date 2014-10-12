include(../common.pri)

TEMPLATE = lib

TARGET = callrecorder

target.path = /usr/share/$${PACKAGE}/lib

INSTALLS += target

QT += core sql

DEFINES += LIBCALLRECORDER

SOURCES += \
    src/database.cpp \
    src/callrecorderexception.cpp \
    src/sqlcursor.cpp \
    src/eventstablemodel.cpp

HEADERS += \
    include/libcallrecorder/database.h \
    include/libcallrecorder/eventstablemodel.h \
    src/config.h \
    src/database.h \
    src/callrecorderexception.h \
    src/sqlcursor.h \
    src/eventstablemodel.h \
    include/libcallrecorder/sqlcursor.h \
    include/libcallrecorder/callrecorderexception.h

