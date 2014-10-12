include(../common.pri)

TARGET = $${PACKAGE}d

target.path = /usr/bin

service.files = $${TARGET}.service
service.path = /usr/lib/systemd/user

INSTALLS += target service

TEMPLATE = app

CONFIG += console link_pkgconfig

QT += core dbus sql multimedia

INCLUDEPATH += ../libcallrecorder/include

LIBS += -L../libcallrecorder -lcallrecorder -lqofono-qt5

PKGCONFIG += flac

SOURCES += \
    src/application.cpp \
    src/main.cpp \
    src/voicecallrecorder.cpp \
    src/phonenumberstablemodel.cpp \
    src/settings.cpp \
    src/eventstablemodel.cpp \
    src/model.cpp

HEADERS += \
    src/application.h \
    src/voicecallrecorder.h \
    src/phonenumberstablemodel.h \
    src/settings.h \
    src/eventstablemodel.h \
    src/model.h

OTHER_FILES += \
    $${TARGET}.service
