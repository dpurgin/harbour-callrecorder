include(common.pri)

TEMPLATE = subdirs

SUBDIRS += \
    daemon \
    ui \
    libcallrecorder \
    libcontacts \
    nemo-qml-plugin-contacts \
    qtpulseaudio

daemon.depends = libcallrecorder qtpulseaudio
nemo-qml-plugin-contacts.depends = libcontacts
ui.depends = libcallrecorder nemo-qml-plugin-contacts

license.files = LICENSE
license.path = /usr/share/$${PACKAGE}

INSTALLS += license

OTHER_FILES = \
    rpm/harbour-callrecorder.yaml \
    rpm/harbour-callrecorder.spec \
    LICENSE
