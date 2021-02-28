#    Call Recorder for SailfishOS
#    Copyright (C) 2014-2016 Dmitriy Purgin <dpurgin@gmail.com>

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

TARGET = $${PACKAGE}

CONFIG += sailfishapp qml_debug
PKGCONFIG += flac

QT += core quick qml multimedia sql

INCLUDEPATH += \
    ../libcallrecorder/include \

LIBS += \
    -L../libcallrecorder -lcallrecorder \
    -Llib -larchive

QMAKE_CXXFLAGS += -std=c++11 -Wall -Werror

HEADERS += \
    src/backupexception.h \
    src/backuphelper.h \
    src/backupworker.h \
    src/databaserepairhelper.h \
    src/databaserepairworker.h \
    src/filerelocationworker.h \
    src/filesystemhelper.h \
    src/localelistmodel.h

SOURCES += \
    src/backupexception.cpp \
    src/backuphelper.cpp \
    src/backupworker.cpp \
    src/databaserepairhelper.cpp \
    src/databaserepairworker.cpp \
    src/filerelocationworker.cpp \
    src/filesystemhelper.cpp \
    src/main.cpp \
    src/localelistmodel.cpp

RESOURCES += ui.qrc

QMLFILES = \
    qml/cover/CoverPage.qml \
    qml/pages/dialogs/EventsFilterDialog.qml \
    qml/pages/dialogs/FileNameDialog.qml \
    qml/pages/dialogs/FilePickerDialog.qml \
    qml/pages/dialogs/ContactPickerDelegate.qml \
    qml/pages/dialogs/ContactPickerDelegateMenu.qml \
    qml/pages/dialogs/ContactPickerDialog.qml \
    qml/pages/dialogs/PhoneNumbersListDelegate.qml \
    qml/pages/dialogs/PhoneNumbersListDialog.qml \
    qml/pages/settings/About.qml \
    qml/pages/settings/AudioSettings.qml \
    qml/pages/settings/Backup.qml \
    qml/pages/settings/BackupFileExists.qml \
    qml/pages/settings/BackupMetaObject.qml \
    qml/pages/settings/BackupMetaWidget.qml \
    qml/pages/settings/BackupRestoreSettings.qml \
    qml/pages/settings/BackupWorker.qml \
    qml/pages/settings/Daemon.qml \
    qml/pages/settings/DatabaseRepairWorker.qml \
    qml/pages/settings/License.qml \
    qml/pages/settings/Storage.qml \
    qml/pages/settings/Translators.qml \
    qml/pages/settings/UserInterface.qml \
    qml/pages/settings/Utilities.qml \
    qml/pages/EventPage.qml \
    qml/pages/EventPageLandscape.qml \
    qml/pages/EventPagePortrait.qml \
    qml/pages/EventsDelegate.qml \
    qml/pages/EventsDelegateContent.qml \
    qml/pages/EventsDelegateMenu.qml \
    qml/pages/EventsPage.qml \
    qml/pages/SettingsPage.qml \
    qml/widgets/PhoneNumberEntryField.qml \
    qml/widgets/StyledLabel.qml \
    qml/main.qml

OTHER_FILES += \
    harbour-callrecorder-launcher.sh \
    harbour-callrecorder.desktop \
    $${QMLFILES}

images.files = \
    images/icon-cover-start-recorder.png \
    images/icon-cover-stop-recorder.png
images.path = /usr/share/$${PACKAGE}/images

scripts.files = \
    harbour-callrecorder-launcher.sh
scripts.path = /usr/bin

INSTALLS += images scripts

lupdate_only {
    SOURCES += $${QMLFILES}
}

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n sailfishapp_i18n_idbased
TRANSLATIONS += \
    translations/ui.ts \
    translations/ui-cs.ts \
    translations/ui-da.ts \
    translations/ui-de.ts \
    translations/ui-de_AT-3.ts \
    translations/ui-el.ts \
    translations/ui-es.ts \
    translations/ui-fi.ts \
    translations/ui-fr.ts \
    translations/ui-hu.ts \
    translations/ui-it.ts \
    translations/ui-lt.ts \
    translations/ui-nl.ts \
    translations/ui-ml.ts \
    translations/ui-pl.ts \
    translations/ui-ru.ts \
    translations/ui-sk.ts \
    translations/ui-sv.ts \
    translations/ui-zh_CN.ts \
    translations/ui-zh_TW.ts
