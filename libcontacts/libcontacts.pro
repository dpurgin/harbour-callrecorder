include(../common.pri)
include(libcontacts/config.pri)

TEMPLATE = lib
CONFIG += qt hide_symbols
#CONFIG += create_pc create_prl no_install_prl

# 'contacts' is too generic for the target name - use 'contactcache'
TARGET = $${PACKAGENAME}
target.path = /usr/share/$${PACKAGE}/lib
INSTALLS += target

# version for generated pkgconfig files is defined in the spec file
#QMAKE_PKGCONFIG_INCDIR = $$PREFIX/include/$${PACKAGENAME}
#QMAKE_PKGCONFIG_LIBDIR = $$PREFIX/lib
#QMAKE_PKGCONFIG_DESTDIR = pkgconfig

CONFIG += link_pkgconfig
packagesExist(mlite5) {
    PKGCONFIG += mlite5
    DEFINES += HAS_MLITE
} else {
    warning("mlite not available. Some functionality may not work as expected.")
}
PKGCONFIG += mlocale5 mce

DEFINES += CONTACTCACHE_BUILD

# We need access to QtContacts private headers
QT += contacts-private

# We need the moc output for ContactManagerEngine from sqlite-extensions
extensionsIncludePath = $$system(pkg-config --cflags-only-I qtcontacts-sqlite-qt5-extensions)
VPATH += $$replace(extensionsIncludePath, -I, )
HEADERS += contactmanagerengine.h

SOURCES += \
    libcontacts/src/cacheconfiguration.cpp \
    libcontacts/src/seasidecache.cpp \
    libcontacts/src/seasideexport.cpp \
    libcontacts/src/seasideimport.cpp \
    libcontacts/src/seasidecontactbuilder.cpp \
    libcontacts/src/seasidepropertyhandler.cpp

HEADERS += \
    libcontacts/src/cacheconfiguration.h \
    libcontacts/src/contactcacheexport.h \
    libcontacts/src/seasidecache.h \
    libcontacts/src/seasideexport.h \
    libcontacts/src/seasideimport.h \
    libcontacts/src/seasidecontactbuilder.h \
    libcontacts/src/synchronizelists.h \
    libcontacts/src/seasidenamegrouper.h \
    libcontacts/src/seasidepropertyhandler.h

#headers.files = \
#    $$PWD/cacheconfiguration.h \
#    $$PWD/contactcacheexport.h \
#    $$PWD/seasidecache.h \
#    $$PWD/seasideexport.h \
#    $$PWD/seasideimport.h \
#    $$PWD/seasidecontactbuilder.h \
#    $$PWD/synchronizelists.h \
#    $$PWD/seasidenamegrouper.h \
#    $$PWD/seasidepropertyhandler.h
#headers.path = $$PREFIX/include/$$TARGET
#INSTALLS += headers
