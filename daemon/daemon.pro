TARGET = harbour-callrecorderd

target.path = /usr/bin

INSTALLS += target

TEMPLATE = app

CONFIG += console

QT += core dbus sql multimedia

LIBS += -lqofono-qt5

SOURCES += \
    src/application.cpp \
    src/main.cpp \
    src/callrecorderexception.cpp \
    src/voicecallrecorder.cpp \
    src/database.cpp \
    src/phonenumberstablemodel.cpp \
    src/sqlcursor.cpp \
    src/settings.cpp \
    src/eventstablemodel.cpp

HEADERS += \
    src/application.h \
    src/callrecorderexception.h \
    src/voicecallrecorder.h \
    src/database.h \
    src/phonenumberstablemodel.h \
    src/sqlcursor.h \
    src/settings.h \
    src/eventstablemodel.h

