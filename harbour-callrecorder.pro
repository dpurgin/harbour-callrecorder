include(common.pri)

TEMPLATE = subdirs

SUBDIRS += \
    daemon \
    ui \
    libcallrecorder

daemon.depends = libcallrecorder
ui.depends = libcallrecorder

license.files = LICENSE
license.path = /usr/share/$${PACKAGE}

INSTALLS += license

OTHER_FILES = \
    rpm/harbour-callrecorder.yaml \
    rpm/harbour-callrecorder.spec \
    LICENSE
