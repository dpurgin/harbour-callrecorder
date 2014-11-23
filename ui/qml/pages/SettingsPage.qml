/*
    Call Recorder for SailfishOS
    Copyright (C) 2014  Dmitriy Purgin <dpurgin@gmail.com>

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
    id: settingsPage

    SilicaFlickable {
        anchors.fill: parent

        contentHeight: contentColumn.height

        Column {
            id: contentColumn

            width: parent.width

            PageHeader {
                title: qsTr('Settings')
            }

            SectionHeader {
                text: qsTr('Recorder Daemon')
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
                text: qsTr('Quality')
            }

            ComboBox {
                label: qsTr('Sample rate')

                menu: ContextMenu {
                    MenuItem {
                        text: '44.1 kHz'
                        property int value: 44100
                    }
                    MenuItem {
                        text: '32 kHz'
                        property int value: 32000
                    }
                    MenuItem {
                        text: '22.05 kHz'
                        property int value: 22050
                    }
                    MenuItem {
                        text: '16 kHz'
                        property int value: 16000
                    }
                    MenuItem {
                        text: '11.025 kHz'
                        property int value: 11025
                    }
                    MenuItem {
                        text: '8 kHz'
                        property int value: 8000
                    }
                }
            }
        }
    }
}
