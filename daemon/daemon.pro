#    Call Recorder for SailfishOS
#    Copyright (C) 2014  Dmitriy Purgin <dpurgin@gmail.com>

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

include(../common.pri)

TARGET = $${PACKAGE}d

target.path = /usr/bin

service.files = $${TARGET}.service
service.path = /usr/lib/systemd/user

scripts.files = $${TARGET}-pre.sh
scripts.path = /usr/bin

INSTALLS += target service scripts

TEMPLATE = app

CONFIG += qml_debug link_pkgconfig

QT += core dbus sql multimedia quick qml gui-private

INCLUDEPATH += \
    ../libcallrecorder/include \
    ../qtpulseaudio/qtpulseaudio/lib/include

LIBS += -L../libcallrecorder -lcallrecorder -L../qtpulseaudio -lqtpulse

PKGCONFIG += flac qofono-qt5

QMLFILES = \
    qml/pages/dialogs/ApprovalDialog.qml \
    qml/widgets/ApprovalDialogButton.qml \
    qml/approval.qml

qml.files = qml
qml.path = /usr/share/$${PACKAGE}

INSTALLS += qml

SOURCES += \
    src/application.cpp \
    src/main.cpp \
    src/voicecallrecorder.cpp \
    src/phonenumberstablemodel.cpp \
    src/model.cpp \
    src/dbusadaptor.cpp \
    src/uidbusinterface.cpp

HEADERS += \
    src/application.h \
    src/voicecallrecorder.h \
    src/phonenumberstablemodel.h \
    src/model.h \
    src/dbusadaptor.h \
    src/uidbusinterface.h

lupdate_only {
    SOURCES += $${QMLFILES}
}

OTHER_FILES += \
    $${TARGET}.service \
    $${TARGET}-pre.sh \
    $${QMLFILES} \

TRANSLATIONS += \
    translations/daemon.ts \
    translations/daemon-cs.ts \
    translations/daemon-da.ts \
    translations/daemon-de.ts \
    translations/daemon-es.ts \
    translations/daemon-fi.ts \
    translations/daemon-fr.ts \
#    translations/daemon-he.ts \
    translations/daemon-nl.ts \
    translations/daemon-pl.ts \
    translations/daemon-ru.ts \
    translations/daemon-sk.ts \
    translations/daemon-sv.ts \
    translations/daemon-zh_TW.ts

qm.path = $${TARGETDIR}/translations
qm.commands += \
    mkdir -p translations; \
    cp --update $${_PRO_FILE_PWD_}/translations/*.ts $${OUT_PWD}/translations/; \
    lrelease $${OUT_PWD}/translations/*.ts;

QMFILES =

for(ts, TRANSLATIONS) {
    QMFILES += $$replace(ts, .ts, .qm)
}

qm.files = $${QMFILES}

INSTALLS += qm



