/*
    Call Recorder for SailfishOS
    Copyright (C) 2016  Dmitriy Purgin <dpurgin@gmail.com>

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

import QtQuick 2.0
import Sailfish.Silica 1.0

import kz.dpurgin.callrecorder.Settings 1.0

import "../../widgets"

Page
{
    allowedOrientations: Orientation.All

    SilicaFlickable
    {
        anchors.fill: parent

        contentHeight: content.height

        VerticalScrollDecorator { }

        Column
        {
            id: content

            width: parent.width

            PageHeader
            {                
                id: header
                //: Page header
                //% "Utilities"
                title: qsTrId("id_settings_utilities")
            }

            SectionHeader
            {
                //: Section header
                //% "Database Repair"
                text: qsTrId("id_database_repair")
            }

            StyledLabel
            {
                height: implicitHeight + Theme.paddingLarge

                //: Description of database repair
                //% "If the list of recordings and file storage became inconsistent (e.g. a recording was removed from the list but still exists in file storage), you should use this tool to fix it."
                text: qsTrId("id_database_repair_description")

                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeExtraSmall
            }

            ComboBox
            {
                id: recordRepairModeCombo

                currentIndex: 1

                //: Combobox label
                //% "Orphaned records"
                label: qsTrId("id_orphaned_records")

                description:
                    currentIndex == 0?
                        //: Description of "Skip" option for orphaned records
                        //% "Entry will be left in the database if there's no corresponding recording file on disk"
                        qsTrId("id_orphaned_records_skip"):
                        //: Description of "Remove" option for orphaned records
                        //% "Entry will be removed from the database if there's no corresponding recording file on disk"
                        qsTrId("id_orphaned_records_remove")

                width: parent.width

                menu: ContextMenu
                {
                    //: "Skip" option for comboboxes (lowercased)
                    //% "skip"
                    MenuItem { text: qsTrId('id_combobox_do_skip') }
                    //: "Remove" option for comboboxes (lowercased)
                    //% "remove"
                    MenuItem { text: qsTrId('id_combobox_do_remove') }
                }
            }

            ComboBox
            {
                id: fileRepairModeCombo

                currentIndex: 1

                //: Combobox label
                //% "Orphaned files"
                label: qsTrId("id_orphaned_files")
                description:
                {
                    switch (currentIndex)
                    {
                        //% "Recording file will be skipped if it's not referenced by an entry in the database"
                        case 0: return qsTrId("id_orphaned_files_skip");
                        //% "Recording file will be removed if it's not referenced by an entry in the database"
                        case 1: return qsTrId("id_orphaned_files_remove");
                        default:
                    }

                    //% "Recording file will be used to restore an entry in the database if there is none"
                    return qsTrId("id_orphaned_files_restore");
                }

                width: parent.width

                menu: ContextMenu
                {
                    //: "skip" option for comboboxes (lowercased)
                    //% "skip"
                    MenuItem { text: qsTrId("id_combobox_do_skip") }
                    //: "remove" option for comboboxes (lowercased)
                    //% "remove"
                    MenuItem { text: qsTrId("id_combobox_do_remove") }
                    //: "restore" option for comboboxes (lowercased)
                    //% "restore"
                    MenuItem { text: qsTrId("id_combobox_do_restore") }
                }
            }        

            Item
            {
                width: parent.width
                height: Theme.paddingLarge
            }

            Button
            {
                anchors.horizontalCenter: parent.horizontalCenter

                //% "Repair"
                text: qsTrId('id_do_repair')

                onClicked:
                {
                    pageStack.push('DatabaseRepairWorker.qml', {
                        fileRepairMode: fileRepairModeCombo.currentIndex,
                        recordRepairMode: recordRepairModeCombo.currentIndex
                    });
                }
            }
        }    
    }
}
