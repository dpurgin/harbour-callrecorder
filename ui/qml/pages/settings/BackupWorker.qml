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

import "../../widgets"

Page
{
    property string fileName
    property bool compress: false
    property bool overwrite: true
    property string outputLocation
    property bool removeExisting: true

    property bool isBackup: true
    readonly property bool isRestore: !isBackup

    readonly property bool restoreComplete:
        isRestore && errorCode === BackupHelper.None && operation === BackupHelper.Complete

    onRestoreCompleteChanged:
    {
        console.log('restore complete: '+ restoreComplete);
    }

    property alias busy: backupHelper.busy
    property alias errorCode: backupHelper.errorCode
    property alias progress: backupHelper.progress
    property alias operation: backupHelper.operation        

    allowedOrientations: Orientation.All
    backNavigation: !busy && !restoreComplete

    states: [
        State
        {
            when: busy && operation !== BackupHelper.NotStarted

            PropertyChanges
            {
                target: progressBar

                label:
                {
                    switch (operation)
                    {
                        //: Backup operation description
                        //% "Preparing..."
                        case BackupHelper.Preparing: return qsTrId("id_preparing");
                        //: Backup operation description
                        //% "Making backup..."
                        case BackupHelper.BackingUp: return qsTrId("id_making_backup");
                        //: Backup operation description
                        //% "Removing old data..."
                        case BackupHelper.RemovingOldData: return qsTrId("id_removing_old_data");
                        //: Backup operation description
                        //% "Restoring..."
                        case BackupHelper.Restoring: return qsTrId("id_restoring");
                        //: Backup operation description
                        //% "Complete!"
                        case BackupHelper.Complete: return qsTrId("id_complete");
                        default:
                    }

                    //: Backup operation description
                    //% "Not started"
                    return qsTrId("id_not_started");
                }
            }
        },

        State
        {
            when: !busy && errorCode === BackupHelper.None && progress > 0

            PropertyChanges
            {
                target: progressBar

                //: Backup operation description
                //% "Complete!"
                label: qsTrId('id_complete')
            }
        },

        State
        {
            when: errorCode !== BackupHelper.None

            PropertyChanges
            {
                target: progressBar

                //: Backup operation description
                //% "Error"
                label: qsTrId("id_error")
            }

            PropertyChanges
            {
                target: progressBar

                indeterminate: false
                value: 100
                maximumValue: 100

                label:
                {
                    switch (errorCode)
                    {
                        //: Backup error description
                        //% "Unable to write archive"
                        case BackupHelper.UnableToWrite: return qsTrId("id_unable_to_write_archive");
                        //: Backup error description
                        //% "Unable to start operation"
                        case BackupHelper.UnableToStart: return qsTrId("id_unable_to_start_operation");
                        //: Backup error description
                        //% "Backup file already exists"
                        case BackupHelper.FileExists: return qsTrId("id_backup_file_exists");
                        //: Backup error description
                        //% "Backup file doesn't exist"
                        case BackupHelper.FileNotExists: return qsTrId("id_backup_file_not_exists");
                    }

                    //: Backup error description
                    //% "Unknown error"
                    return qsTrId("id_unknown_error");
                }
            }
        }

    ]

    BackupHelper
    {
        id: backupHelper
    }

    Column
    {
        anchors.fill: parent

        PageHeader
        {
            title: isBackup?
                       //% "Backup"
                       qsTrId("id_settings_backup"):
                       //% "Restore"
                       qsTrId('id_restore')
        }

        StyledLabel
        {
            text: isBackup?
                      //% "Performing backup. Please do not close the application until the operation is complete."
                      qsTrId("id_performing_backup_description"):
                      //% "Performing restore. Please do not close the application until the operation is complete. Doing so may damage the data completely and lead to unpredictable behavior."
                      qsTrId("id_performing_restore_description")


            height: implicitHeight + Theme.paddingLarge

            color: Theme.highlightColor
            font.pixelSize: Theme.fontSizeSmall
        }

        ProgressBar
        {
            id: progressBar

            width: parent.width

            indeterminate: progress <= 1

            minimumValue: 0
            maximumValue: backupHelper.totalCount
            value: backupHelper.progress
        }

        Item
        {
            width: parent.width
            height: Theme.paddingLarge * 2
        }

        StyledLabel
        {
            //% "Please restart the application"
            text: qsTrId("id_restart_app_description")

            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter

            font.pixelSize: Theme.fontSizeSmall

            visible: restoreComplete
            opacity: visible? 1: 0

            Behavior on opacity
            {
                FadeAnimator { }
            }
        }

        StyledLabel
        {
            //% "Database, settings and recordings were successfully restored. The Call Recorder needs to be restarted to apply the changes. If you chose to merge the existing recording files, you should run the database repair tool from Utilities after restart."
            text: qsTrId("id_restore_success_description");

            color: Theme.secondaryColor

            height: implicitHeight + Theme.paddingMedium

            font.pixelSize: Theme.fontSizeExtraSmall

            visible: restoreComplete
            opacity: visible? 1: 0

            Behavior on opacity {
                FadeAnimator { }
            }
        }
    }

    onStatusChanged:
    {
        if (status === PageStatus.Activating)
        {
            if (isBackup)
                backupHelper.backup(fileName, compress, overwrite);
            else if (isRestore)
                backupHelper.restore(fileName, outputLocation, removeExisting);
        }
    }
}
