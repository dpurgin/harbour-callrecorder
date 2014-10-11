# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

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
    qml/cover/CoverPage.qml \
    qml/pages/EventsPage.qml \
    qml/pages/EventPage.qml \
    qml/main.qml

# to disable building translations every time, comment out the
# following CONFIG line
#CONFIG += sailfishapp_i18n
#TRANSLATIONS += translations/ui-de.ts

