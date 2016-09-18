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

import "../../widgets"

Dialog
{
    property alias backupMeta: backupMetaWidget.backupMeta

    allowedOrientations: Orientation.All

    DialogHeader
    {
        id: header
    }

    SilicaFlickable
    {
        anchors
        {
            top: header.bottom
            right: parent.right
            bottom: parent.bottom
            left: parent.left
        }

        contentHeight: content.height

        Column
        {
            id: content

            width: parent.width

            SectionHeader
            {
                //: Backup page section header
                //% "Backupo Information"
                text: qsTrId("id_backup_information")
            }

            BackupMetaWidget
            {
                id: backupMetaWidget
            }

            Item
            {
                width: parent.width
                height: Theme.paddingLarge
            }

            SectionHeader
            {
                //: Backup page secion header
                //% "Restore Settings"
                text: qsTrId("id_restore_settings")
            }

            TextSwitch
            {
                //: Restore settings
                //% "Do not remove existing recordings"
                text: qsTrId("id_dont_remove_existing_recordings")
                //: Restore description
                //% "Leave all existing audio files intact, you can add them to UI later using the database repair tool in Utilities"
                description: qsTrId("id_dont_remove_existing_recordigns_description")

                width: parent.width

                onCheckedChanged:
                {
                    acceptDestinationProperties['removeExisting'] = !checked;
                }
            }
        }
    }
}
