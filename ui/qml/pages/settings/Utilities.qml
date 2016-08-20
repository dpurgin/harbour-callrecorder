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
                title: qsTr('Utilities')
            }

            SectionHeader
            {
                text: qsTr('Database Repair')
            }

            StyledLabel
            {
                height: implicitHeight + Theme.paddingLarge

                text: qsTr('If the list of recordings and file storage became inconsistent ' +
                           '(e.g. a recording was removed from the list but still exists in file storage), ' +
                           'you should use this tool to fix it')

                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeExtraSmall
            }

            ComboBox
            {
                id: recordRepairModeCombo

                currentIndex: 1

                label: qsTr('Orphaned records')

                description:
                    currentIndex == 0?
                        qsTr('Entry will be left in the database if there\'s no corresponding recording file on disk'):
                        qsTr('Entry will be removed from the database if there\'s no corresponding recording file on disk')

                width: parent.width

                menu: ContextMenu
                {
                    MenuItem { text: qsTr('skip') }
                    MenuItem { text: qsTr('remove') }
                }
            }

            ComboBox
            {
                id: fileRepairModeCombo

                currentIndex: 1

                label: qsTr('Orphaned files')
                description:
                {
                    switch (currentIndex)
                    {
                        case 1: return qsTr('Recording file will be removed if it\'s not referenced by an entry in the database');
                        case 2: return qsTr('Recording file will be used to restore an entry in the database if there is none');
                        default:
                    }

                    return qsTr('Recording file will be skipped if it\'s not referenced by an entry in the database');
                }

                width: parent.width

                menu: ContextMenu
                {
                    MenuItem { text: qsTr('skip') }
                    MenuItem { text: qsTr('remove') }
                    MenuItem { text: qsTr('restore') }
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

                text: qsTr('Repair')

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
