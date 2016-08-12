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

Page
{
    Column
    {
        anchors.fill: parent

        PageHeader
        {
            title: qsTr('Backup')
        }

        Label
        {
            text: qsTr('Create a backup to save the recorded calls and settings. Use it later to restore the data on other device or after factory reset.')

            x: Theme.horizontalPageMargin
            width: parent.width - x * 2
            height: implicitHeight + Theme.paddingLarge

            color: Theme.highlightColor
            font.pixelSize: Theme.fontSizeSmall
            wrapMode: Text.Wrap
        }

        TextField
        {
            id: backupPath

            text: StandardPaths.documents + '/' + generateBackupFileName()

            label: qsTr('Backup file name')

            width: parent.width

            errorHighlight: !fileSystemHelper.isWritable(fileSystemHelper.absolutePath(text))
        }

        Button
        {
            text: qsTr('Browse')

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

            text: qsTr('Compress backup')
            description: qsTr('The backup will be compressed using BZIP2. This slows down the operation significantly and doesn\'t bring much if FLAC compression was already set to maximum in Audio settings.')

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
            text: qsTr('Backup')

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
