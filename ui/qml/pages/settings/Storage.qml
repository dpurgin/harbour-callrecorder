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

import kz.dpurgin.callrecorder.Settings 1.0

Page {
    property bool acceptChanges: false

    SilicaFlickable {
        anchors.fill: parent

        contentHeight: content.height

        Column {
            id: content

            width: parent.width

            PageHeader {
                title: qsTr('Storage')
            }

            SectionHeader {
                text: qsTr('Location')
            }

            TextField {
                id: outputLocationField

                label: qsTr('Location for storing the recordings')

                width: parent.width

                text: settings.outputLocation

                errorHighlight: !fileSystemHelper.isWritable(text)
            }

            Row {
                anchors {
                    horizontalCenter: parent.horizontalCenter

                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }

                Button {
                    text: qsTr('Browse')

                    onClicked: {
                        var dlg = pageStack.push('../DirectoryPickerDialog.qml', {
                            directoryPath: outputLocationField.text
                        });

                        dlg.accepted.connect(function() {
                            outputLocationField.text = dlg.directoryPath;
                        })
                    }
                }

                Button {
                    text: qsTr('Save')

                    enabled: !outputLocationField.errorHighlight &&
                                (settings.outputLocation !== outputLocationField.text)

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
                label: qsTr('Relocating files')

                width: parent.width

                minimumValue: 0

                visible: fileSystemHelper.busy
                value: fileSystemHelper.progress
                maximumValue: fileSystemHelper.totalCount
            }

            SectionHeader {
                text: qsTr('Storage limits')

                anchors.rightMargin: Theme.paddingLarge
            }

            TextSwitch {
                text: qsTr('Limit storage by size or age')

                checked: settings.limitStorage

                onCheckedChanged: {
                    settings.limitStorage = checked;
                }
            }

            ComboBox {
                id: storageAgeCombo

                label: qsTr('By age')

                visible: settings.limitStorage

                opacity: visible? 1: 0

                Behavior on opacity {
                    FadeAnimation { }
                }

                menu: ContextMenu {
                    MenuItem {
                        property int value: 0

                        text: qsTr('no limit')
                    }

                    MenuItem {
                        property int value: 30

                        text: qsTr('30 days')
                    }

                    MenuItem {
                        property int value: 90

                        text: qsTr('90 days')
                    }

                    MenuItem {
                        property int value: 180

                        text: qsTr('180 days')
                    }

                    MenuItem {
                        property int value: 365

                        text: qsTr('365 days')
                    }

                    MenuItem {
                        property int value: -1

                        text: qsTr('Custom')
                    }
                }

                onCurrentItemChanged: {
                    if (acceptChanges)
                    {
                        settings.maxStorageAge = (currentItem.value === -1? 365: currentItem.value);
                    }
                }
            }

            TextField {
                label: qsTr('Custom age limit in days')

                placeholderText: qsTr('Custom age limit in days')

                text: settings.maxStorageAge

                inputMethodHints: Qt.ImhDigitsOnly

                width: parent.width

                visible: settings.limitStorage && storageAgeCombo.currentItem.value === -1

                opacity: visible? 1: 0

                Behavior on opacity {
                    FadeAnimation { }
                }

                validator: IntValidator {
                    bottom: 1
                }

                onTextChanged: {
                    if (acceptableInput)
                        settings.maxStorageAge = text;
                }
            }

            ComboBox {
                id: storageSizeCombo

                label: qsTr('By size')

                visible: settings.limitStorage

                opacity: visible? 1: 0

                Behavior on opacity {
                    FadeAnimation { }
                }

                menu: ContextMenu {
                    MenuItem {
                        property int value: 0

                        text: qsTr('no limit')
                    }

                    MenuItem {
                        property int value: 300

                        text: qsTr('300 MB')
                    }

                    MenuItem {
                        property int value: 500

                        text: qsTr('500 MB')
                    }

                    MenuItem {
                        property int value: 1024

                        text: qsTr('1 GB')
                    }

                    MenuItem {
                        property int value: 3072

                        text: qsTr('3 GB')
                    }

                    MenuItem {
                        property int value: 5120

                        text: qsTr('5 GB')
                    }

                    MenuItem {
                        property int value: -1

                        text: qsTr('Custom')
                    }
                }

                onCurrentItemChanged: {
                    if (acceptChanges)
                    {
                        settings.maxStorageSize =
                                (currentItem.value === -1?
                                     1024:
                                     currentItem.value);
                    }
                }
            }

            TextField {
                label: qsTr('Custom size limit in MB')

                placeholderText: qsTr('Custom size limit in MB')

                text: settings.maxStorageSize

                inputMethodHints: Qt.ImhDigitsOnly

                width: parent.width

                visible: settings.limitStorage && storageSizeCombo.currentItem.value === -1

                opacity: visible? 1: 0

                Behavior on opacity {
                    FadeAnimation { }
                }

                validator: IntValidator {
                    bottom: 1
                }

                onTextChanged: {
                    if (acceptableInput)
                        settings.maxStorageSize = text;
                }
            }
        }
    }

    RemorsePopup {
        id: remorse
    }

    Component.onCompleted: {
        storageAgeCombo.menu._foreachMenuItem(function(item, index) {
            if (item.value === settings.maxStorageAge || item.value === -1)
            {
                storageAgeCombo.currentIndex = index;
                return false;
            }

            return true;
        });

        storageSizeCombo.menu._foreachMenuItem(function(item, index) {
            if (item.value === settings.maxStorageSize || item.value === -1)
            {
                storageSizeCombo.currentIndex = index;
                return false;
            }

            return true;
        });

        acceptChanges = true;
    }
}
