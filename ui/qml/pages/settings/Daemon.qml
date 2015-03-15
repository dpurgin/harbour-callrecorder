/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2015 Dmitriy Purgin <dpurgin@gmail.com>

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

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content

            anchors.fill: parent

            PageHeader {
                id: pageHeader

                title: qsTr('Recording daemon')
            }

            SectionHeader {
                text: qsTr('Startup')
            }

            TextSwitch {
                id: activeSwitch

                automaticCheck: false

                checked: systemdUnit.isActive

                text: qsTr('Active')
                description: qsTr('Capture all incoming and outgoing calls')

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

                text: qsTr('Automatic startup')
                description: qsTr('Start automatically upon reboot')

                onClicked: {
                    if (systemdUnit.isEnabled)
                        systemdUnit.disable();
                    else
                        systemdUnit.enable();
                }
            }

            SectionHeader {
                text: qsTr('Operation mode')
            }

            ComboBox {
                id: operationModeCombo

                label: qsTr('Operation mode')

                description:
                    currentIndex == 0?
                        qsTr('Record calls to any number except those in the black list'):
                        qsTr('Do not record anything except numbers in the white list')

                menu: ContextMenu {
                    MenuItem { text: qsTr('Black list') }
                    MenuItem { text: qsTr('White list') }
                }

                onCurrentIndexChanged: {
                    if (acceptChanges)
                    {
                        settings.operationMode = currentIndex;
                    }
                }
            }

            Button {
                text: qsTr('Edit list')

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
