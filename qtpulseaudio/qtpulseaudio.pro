include(../common.pri)

TEMPLATE = lib

TARGET = qtpulse

target.path = /usr/share/$${PACKAGE}/lib

INSTALLS += target

VERSION = 0.1

CONFIG += link_pkgconfig plugin

PKGCONFIG += libpulse

DEFINES += QTPULSEAUDIO

INCLUDEPATH += qtpulseaudio/lib/include

QT -= gui
QT += core

SOURCES += \
    qtpulseaudio/lib/src/qtpulseaudio.cpp \
    qtpulseaudio/lib/src/qtpulseaudiocard.cpp \
    qtpulseaudio/lib/src/qtpulseaudiodata.cpp \
    qtpulseaudio/lib/src/qtpulseaudiocardprivate.cpp \
    qtpulseaudio/lib/src/qtpulseaudiocardprofile.cpp \
    qtpulseaudio/lib/src/qtpulseaudioconnection.cpp \
    qtpulseaudio/lib/src/qtpulseaudioconnectionprivate.cpp \
    qtpulseaudio/lib/src/qtpulseaudiofacility.cpp \
    qtpulseaudio/lib/src/qtpulseaudiofacilityfactory.cpp \
    qtpulseaudio/lib/src/qtpulseaudiofacilityprivate.cpp \
    qtpulseaudio/lib/src/qtpulseaudioserver.cpp \
    qtpulseaudio/lib/src/qtpulseaudioserverprivate.cpp \
    qtpulseaudio/lib/src/qtpulseaudiosink.cpp \
    qtpulseaudio/lib/src/qtpulseaudiosinkport.cpp \
    qtpulseaudio/lib/src/qtpulseaudiosinkprivate.cpp \
    qtpulseaudio/lib/src/qtpulseaudiosource.cpp \
    qtpulseaudio/lib/src/qtpulseaudiosourceprivate.cpp \
    qtpulseaudio/lib/src/qtpulseaudiostream.cpp

HEADERS += \
    qtpulseaudio/lib/src/qtpulseaudiodata.h \
    qtpulseaudio/lib/include/qtpulseaudio/qtpulseaudio.h \
    qtpulseaudio/lib/include/qtpulseaudio/qtpulseaudiocard.h \
    qtpulseaudio/lib/include/qtpulseaudio/qtpulseaudiocardprofile.h \
    qtpulseaudio/lib/include/qtpulseaudio/qtpulseaudioconfig.h \
    qtpulseaudio/lib/include/qtpulseaudio/qtpulseaudioconnection.h \
    qtpulseaudio/lib/include/qtpulseaudio/qtpulseaudiofacility.h \
    qtpulseaudio/lib/include/qtpulseaudio/qtpulseaudioserver.h \
    qtpulseaudio/lib/include/qtpulseaudio/qtpulseaudiosink.h \
    qtpulseaudio/lib/include/qtpulseaudio/qtpulseaudiosinkport.h \
    qtpulseaudio/lib/include/qtpulseaudio/qtpulseaudiosource.h \
    qtpulseaudio/lib/include/qtpulseaudio/qtpulseaudiostream.h \
    qtpulseaudio/lib/src/qtpulseaudiocardprivate.h \
    qtpulseaudio/lib/src/qtpulseaudioconnectionprivate.h \
    qtpulseaudio/lib/src/qtpulseaudiofacilityfactory.h \
    qtpulseaudio/lib/src/qtpulseaudiofacilityprivate.h \
    qtpulseaudio/lib/src/qtpulseaudioserverprivate.h \
    qtpulseaudio/lib/src/qtpulseaudiosinkprivate.h \
    qtpulseaudio/lib/src/qtpulseaudiosourceprivate.h \
