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

    property bool acceptChanges: false // this is set to true when the page is complete, any change is made by the user

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
                text: qsTr('Location')
            }

            TextField {
                id: outputLocationField

                width: parent.width

                text: settings.outputLocation

                errorHighlight: !fileSystemHelper.isWritable(text)

                label: qsTr('Location for storing the recordings')
            }

            Row {
                width: parent.width

                anchors {
                    left: parent.left
                    right: parent.right

                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge

                    horizontalCenter: parent.horizontalCenter
                }

                Button {
                    text: qsTr('Browse')

                    onClicked: {
                        var dlg = pageStack.push("DirectoryPickerDialog.qml", {
                            directoryPath: outputLocationField.text
                        });

                        dlg.accepted.connect(function() {
                            outputLocationField.text = dlg.directoryPath;
                        })
                    }
                }

                Button {
                    text: qsTr('Save')

                    enabled: !outputLocationField.errorHighlight && (settings.outputLocation !== outputLocationField.text)

                    onClicked: {
                        var oldLocation = settings.outputLocation;
                        var newLocation = outputLocationField.text;

                        settings.outputLocation = newLocation;

                        if (!fileSystemHelper.dirIsEmpty(oldLocation))
                        {
                            remorse.execute(qsTr("Relocating files"), function() {
                                fileSystemHelper.relocate(oldLocation, newLocation);
                            });
                        }
                    }
                }
            }

            ProgressBar {
                id: relocationProgress

                width: parent.width

                label: qsTr('Relocating files')

                minimumValue: 0

                visible: fileSystemHelper.busy
                value: fileSystemHelper.progress
                maximumValue: fileSystemHelper.queueLength
            }

            SectionHeader {
                text: qsTr('Record Quality')
            }

            ComboBox {
                id: sampleRateCombo

                label: qsTr('Sample rate')

                menu: ContextMenu {
                    id: sampleRateMenu

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

                onCurrentItemChanged: {
                    if (acceptChanges)
                        settings.sampleRate = currentItem.value;
                }
            }
        }
    }

    Component.onCompleted: {
        console.log('Component.onCompleted');

        sampleRateMenu._foreachMenuItem(function(item, index) {
            if (item.value == settings.sampleRate)
            {
                sampleRateCombo.currentIndex = index;
                return false;
            }

            return true;
        })

        acceptChanges = true;
    }

    RemorsePopup {
        id: remorse
    }
}
