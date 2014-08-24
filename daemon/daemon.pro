TARGET = harbour-callrecorderd

target.path = /usr/bin

INSTALLS += target

TEMPLATE = app

CONFIG += console link_pkgconfig

QT += core dbus sql multimedia

LIBS += -lqofono-qt5

PKGCONFIG += flac

SOURCES += \
    src/application.cpp \
    src/main.cpp \
    src/callrecorderexception.cpp \
    src/voicecallrecorder.cpp \
    src/database.cpp \
    src/phonenumberstablemodel.cpp \
    src/sqlcursor.cpp \
    src/settings.cpp \
    src/eventstablemodel.cpp \
    src/model.cpp

HEADERS += \
    src/application.h \
    src/callrecorderexception.h \
    src/voicecallrecorder.h \
    src/database.h \
    src/phonenumberstablemodel.h \
    src/sqlcursor.h \
    src/settings.h \
    src/eventstablemodel.h \
    src/model.h

OTHER_FILES += \
    harbour-callrecorderd.service
