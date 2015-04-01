include(../common.pri)
include(nemo-qml-plugin-contacts/config.pri)

TARGET = nemocontacts
PLUGIN_IMPORT_PATH = kz/dpurgin/nemomobile/contacts

TEMPLATE = lib
CONFIG += qt plugin

QT += qml contacts
PKGCONFIG += mlocale5

packagesExist(mlite5) {
    PKGCONFIG += mlite5
    DEFINES += HAS_MLITE
} else {
    warning("mlite not available. Some functionality may not work as expected.")
}

target.path = /usr/share/$${PACKAGE}/lib/imports/$${PLUGIN_IMPORT_PATH}
INSTALLS += target

qmldir.files += nemo-qml-plugin-contacts/src/qmldir
qmldir.path +=  /usr/share/$${PACKAGE}/lib/imports/$${PLUGIN_IMPORT_PATH}
INSTALLS += qmldir

SOURCES += nemo-qml-plugin-contacts/src/plugin.cpp \
           nemo-qml-plugin-contacts/src/seasideperson.cpp \
           nemo-qml-plugin-contacts/src/seasidefilteredmodel.cpp \
           nemo-qml-plugin-contacts/src/seasidenamegroupmodel.cpp \
           nemo-qml-plugin-contacts/src/seasidevcardmodel.cpp

HEADERS += nemo-qml-plugin-contacts/src/seasideperson.h \
           nemo-qml-plugin-contacts/src/seasidefilteredmodel.h \
           nemo-qml-plugin-contacts/src/seasidenamegroupmodel.h \
           nemo-qml-plugin-contacts/src/seasidevcardmodel.h

INCLUDEPATH += nemo-qml-plugin-contacts/src \
               ../libcontacts/libcontacts/src

LIBS += -L../libcontacts -lcontactcache-qt5
