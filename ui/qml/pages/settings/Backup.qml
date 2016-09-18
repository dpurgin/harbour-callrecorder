/*
    Call Recorder for SailfishOS
    Copyright (C) 2016 Dmitriy Purgin <dpurgin@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.2

import Sailfish.Silica 1.0

import kz.dpurgin.callrecorder.BackupHelper 1.0
import kz.dpurgin.callrecorder.Settings 1.0

import "../../widgets"

Page
{
    allowedOrientations: Orientation.All

    BackupHelper { id: backupHelper }
    BackupHelper
    {
        id: restoreHelper

        onBackupMetaChanged:
        {
            restoreMeta.fileName = restorePath.text;
            restoreMeta.parse(backupMeta);
        }

        onErrorCodeChanged:
        {
            if (errorCode !== BackupHelper.None)
                restoreMeta.reset();
        }
    }

    BackupMetaObject
    {
        id: restoreMeta

        onChanged: restoreMetaWidget.backupMeta = restoreMeta
    }

    Settings
    {
        id: settings
    }

    SilicaFlickable
    {
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge

        VerticalScrollDecorator { }

        Column
        {
            id: content
            width: parent.width

            PageHeader
            {
                //:
                //% "Backup"
                title: qsTrId('id_settings_backup')
            }

            StyledLabel
            {
                //: Description of backup page
                //% "Create a backup to save the recorded calls and settings. Use it later to restore the data on other device or after factory reset."
                text: qsTrId('id_backup_description')

                height: implicitHeight + Theme.paddingLarge

                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeSmall
            }

            Item
            {
                x: Theme.horizontalPageMargin
                width: parent.width - x * 2
                height: backupBusyIndicator.height + Theme.paddingLarge

                BusyIndicator
                {
                    id: backupBusyIndicator

                    anchors.left: parent.left

                    size: BusyIndicatorSize.Small
                    running: backupHelper.estimatedBackupSize < 0
                }

                Label
                {
                    text: qsTr('Estimating backup size...')

                    anchors
                    {
                        left: backupBusyIndicator.right
                        right: parent.right

                        leftMargin: Theme.paddingMedium

                    }

                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.highlightColor
                    wrapMode: Text.Wrap

                    opacity: backupHelper.estimatedBackupSize < 0? 1: 0

                    Behavior on opacity
                    {
                        FadeAnimation { }
                    }
                }

                Label
                {
                    //: Information label on Backup page
                    //% "Estimated backup size: %1"
                    text: qsTrId('id_estimated_backup_size').arg(
                              Format.formatFileSize(backupHelper.estimatedBackupSize))

                    anchors
                    {
                        left: backupBusyIndicator.right
                        right: parent.right

                        leftMargin: Theme.paddingMedium
                    }

                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.highlightColor
                    wrapMode: Text.Wrap

                    opacity: backupHelper.estimatedBackupSize < 0? 0: 1

                    Behavior on opacity
                    {
                        FadeAnimation { }
                    }
                }
            }

            TextField
            {
                id: backupPath

                text: StandardPaths.documents + '/' + generateBackupFileName()

                //% "Backup file name"
                label: qsTrId('id_backup_file_name')
                //% "Backup file name"
                placeholderText: qsTrId('id_backup_file_name')

                width: parent.width

                errorHighlight: !fileSystemHelper.isWritable(fileSystemHelper.absolutePath(text))
            }

            Button
            {
                //% "Browse"
                text: qsTrId('id_do_browse')

                anchors.horizontalCenter: parent.horizontalCenter

                onClicked:
                {
                    var dlg = pageStack.push('../DirectoryPickerDialog.qml', {
                        directoryPath: fileSystemHelper.absolutePath(backupPath.text)
                    });

                    dlg.accepted.connect(function() {
                        backupPath.text = dlg.directoryPath + '/' + generateBackupFileName();
                    });
                }
            }

            TextSwitch
            {
                id: compressSwitch

                //: Switch control whether compress backup or not
                //% "Compress backup"
                text: qsTrId('id_compress_backup')
                //: Description of the switch
                //% "The backup will be compressed using BZIP2. This slows down the operation significantly and doesn\'t bring much if FLAC compression was already set to maximum in Audio settings."
                description: qsTrId('id_compress_backup_description')

                onCheckedChanged:
                {
                    if (checked && !endsWith(backupPath.text, '.bz2'))
                        backupPath.text += '.bz2';
                    else if (!checked && endsWith(backupPath.text, '.bz2'))
                        backupPath.text = backupPath.text.substring(0, backupPath.text.length - 4);
                }
            }

            Item
            {
                width: parent.width
                height: Theme.paddingLarge
            }

            Button
            {
                //: Backup section header
                //% "Backup"
                text: qsTrId('id_settings_backup')

                anchors.horizontalCenter: parent.horizontalCenter

                enabled: !backupPath.errorHighlight

                onClicked:
                {
                    var config = {
                        fileName: backupPath.text,
                        compress: compressSwitch.checked,
                        overwrite: true
                    };

                    if (fileSystemHelper.exists(backupPath.text))
                    {
                        pageStack.push(Qt.resolvedUrl('BackupFileExists.qml'), {
                            acceptDestination: Qt.resolvedUrl('BackupWorker.qml'),
                            acceptDestinationAction: PageStackAction.Replace,
                            acceptDestinationProperties: config,

                            fileName: backupPath.text
                        });
                    }
                    else
                        pageStack.push(Qt.resolvedUrl('BackupWorker.qml'), config);
                }
            }

            Item
            {
                width: parent.width
                height: Theme.paddingLarge
            }

            SectionHeader
            {
                //: Restore section header
                //% "Restore"
                text: qsTrId('id_restore')
            }

            StyledLabel
            {
                //: Restore description
                //% "Select a backup file made by the Call Recorder previously to restore it to this device."
                text: qsTrId('id_restore_description')

                height: implicitHeight + Theme.paddingLarge

                font.pixelSize: Theme.fontSizeSmall
                color: Theme.highlightColor
            }

            TextField
            {
                id: restorePath

                //: Label for entering path to restore file
                //% "Restore file name"
                label: qsTrId('id_restore_file_name')
                //% "Restore file name"
                placeholderText: qsTrId('id_restore_file_name')

                width: parent.width

                onTextChanged: restoreHelper.readBackupMeta(text);
            }

            Button
            {
                text: qsTr('Browse')

                anchors.horizontalCenter: parent.horizontalCenter

                onClicked:
                {
                    var dlg = pageStack.push(Qt.resolvedUrl('../dialogs/FilePickerDialog.qml'));

                    dlg.accepted.connect(function() {
                        restorePath.text = dlg.absoluteFilePath;
                    })
                }
            }

            Item
            {
                width: parent.width
                height: Theme.paddingLarge
            }

            Row
            {
                x: Theme.horizontalPageMargin

                spacing: Theme.paddingMedium

                width: parent.width - x * 2

                visible: restoreHelper.busy || restoreHelper.errorCode !== BackupHelper.None

                BusyIndicator
                {
                    id: restoreBusyIndicator

                    size: BusyIndicatorSize.Small
                    running: restoreHelper.busy
                }

                Label
                {
                    id: restoreStateLabel

                    width: parent.width - restoreBusyIndicator.width - Theme.paddingMedium

                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.highlightColor
                    wrapMode: Text.Wrap

                    states:
                    [
                        State
                        {
                            when: restoreHelper.busy

                            PropertyChanges
                            {
                                target: restoreStateLabel

                                //: Information label
                                //% "Checking backup file..."
                                text: qsTrId('id_checking_backup_file')
                            }
                        },

                        State
                        {
                            when: restoreHelper.errorCode !== BackupHelper.None

                            PropertyChanges
                            {
                                target: restoreStateLabel

                                //: Information label
                                //% "Selected file is not a valid Call Recorder backup"
                                text: qsTrId('id_backup_invalid')
                            }
                        }
                    ]
                }
            }

            BackupMetaWidget
            {
                id: restoreMetaWidget

                visible: restoreHelper.backupMeta !== '' &&
                         restoreHelper.errorCode === BackupHelper.None
            }

            Item
            {
                width: parent.width
                height: Theme.paddingLarge
            }

            Button
            {
                //: Action button
                //% "Restore"
                text: qsTrId('id_do_restore')

                anchors.horizontalCenter: parent.horizontalCenter

                enabled: restoreHelper.errorCode === BackupHelper.None &&
                         restoreMeta.restoreSize > 0

                onClicked:
                {
                    pageStack.push('BackupRestoreSettings.qml', {
                        acceptDestination: Qt.resolvedUrl('BackupWorker.qml'),
                        acceptDestinationAction: PageStackAction.Replace,
                        acceptDestinationProperties: {
                            isBackup: false,
                            fileName: restoreMeta.fileName,
                            outputLocation: settings.outputLocation
                        },

                        backupMeta: restoreMeta
                    });
                }
            }
        }
    }

    onStatusChanged:
    {
        if (status == PageStatus.Active && backupHelper.estimatedBackupSize < 0)
            backupHelper.estimateBackupSize();
    }

    function endsWith(str, ends)
    {
        return str.substring(str.length - ends.length) === ends;
    }

    function generateBackupFileName()
    {
        var dt = new Date();
        var dtStr =  '' + dt.getFullYear() +
                padLeft(dt.getMonth(), '0', 2) +
                padLeft(dt.getDate(), '0', 2);

        return 'callrecorder-' + dtStr + '.tar' + (compressSwitch.checked? '.bz2': '');
    }

    function padLeft(str, ch, length)
    {
        return (new Array(length - String(str).length + 1)).join(ch) + str;
    }
}
