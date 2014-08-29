TEMPLATE = subdirs

SUBDIRS += \
    daemon \
    ui \
    libcallrecorder

daemon.depends = libcallrecorder
ui.depends = libcallrecorder

OTHER_FILES = \
    rpm/harbour-callrecorder.yaml \
    rpm/harbour-callrecorder.spec
