include(common.pri)

TEMPLATE = subdirs

SUBDIRS += \
    daemon \
    ui \
    libcallrecorder \
    qtpulseaudio

daemon.depends = libcallrecorder qtpulseaudio
ui.depends = libcallrecorder

license.files = LICENSE
license.path = /usr/share/$${PACKAGE}

INSTALLS += license

OTHER_FILES = \
    rpm/harbour-callrecorder.yaml \
    rpm/harbour-callrecorder.spec \
    LICENSE
