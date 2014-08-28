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

QT += core quick qml multimedia

SOURCES += \
    src/main.cpp

OTHER_FILES += \
    qml/cover/CoverPage.qml \
    qml/pages/SecondPage.qml \
    translations/*.ts \
    qml/harbour-callrecorder.qml \
    harbour-callrecorder.desktop \
    qml/pages/EventsPage.qml

# to disable building translations every time, comment out the
# following CONFIG line
#CONFIG += sailfishapp_i18n
#TRANSLATIONS += translations/ui-de.ts

