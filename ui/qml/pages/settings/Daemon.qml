/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2016 Dmitriy Purgin <dpurgin@gmail.com>

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

Page {
    property bool acceptChanges: false; // set to true when the page is initialized and settings are
                                        // actually changed by the user

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        VerticalScrollDecorator {}

        Column {
            id: content

            width: parent.width

            PageHeader {
                id: pageHeader

                //: Page header
                //% "Recording daemon"
                title: qsTrId('id_settings_recording_daemon')
            }

            SectionHeader {
                //: Section header
                //% "Startup"
                text: qsTrId('id_settings_startup')
            }

            TextSwitch {
                id: activeSwitch

                automaticCheck: false

                checked: systemdUnit.isActive

                //: Used on switches
                //% "Active"
                text: qsTrId('id_active')
                //: Description of "Active" switch on Recording Daemon page
                //% "Capture all incoming and outgoing calls"
                description: qsTrId('id_daemon_active_description')

                onClicked: {
                    if (systemdUnit.isActive)
                        systemdUnit.stop();
                    else
                        systemdUnit.start();
                }
            }

            TextSwitch {
                id: startupTypeSwitch

                automaticCheck: false

                checked: systemdUnit.isEnabled

                //: Settings item on Recording Daemon page
                //% "Automatic startup"
                text: qsTrId('id_automatic_startup')
                //: Automatic startup description
                //% "Start automatically upon reboot"
                description: qsTrId('id_automatic_startup_description')

                onClicked: {
                    if (systemdUnit.isEnabled)
                        systemdUnit.disable();
                    else
                        systemdUnit.enable();
                }
            }

            SectionHeader {
                //: Settings item on Recording Daemon page
                //% "Operation mode"
                text: qsTrId('id_operation_mode')
            }

            ComboBox {
                id: operationModeCombo

                //: Settings item on Recording Daemon page
                //% "Operation mode"
                label: qsTrId('id_operation_mode')

                description:
                    currentIndex == 0?
                        //: Black list description
                        //% "Record calls to any number except those in the black list"
                        qsTrId('id_black_list_description'):
                        //: White list description
                        //% "Do not record anything except numbers in the white list"
                        qsTrId('id_white_list_description')

                menu: ContextMenu {
                    MenuItem {
                        //: Operation mode
                        //% "Black list"
                        text: qsTrId('id_black_list')
                    }
                    MenuItem {
                        //: Operation mode
                        //% "White list"
                        text: qsTrId('id_white_list')
                    }
                }

                onCurrentIndexChanged: {
                    if (acceptChanges)
                    {
                        settings.operationMode = currentIndex;
                    }
                }
            }

            Button {
                //: Edit black or white list
                //% "Edit list"
                text: qsTrId('id_do_edit_list')

                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    pageStack.push('../dialogs/PhoneNumbersListDialog.qml', {
                        role: operationModeCombo.currentIndex
                    })
                }
            }
        }
    }

    Component.onCompleted: {
        operationModeCombo.currentIndex = settings.operationMode;

        acceptChanges = true
    }
}
