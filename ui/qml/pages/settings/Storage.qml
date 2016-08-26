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
    along with this program.  If not, see <http://www.gnu.org/licenses/f>.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0

import kz.dpurgin.callrecorder.Settings 1.0

Page {
    property bool acceptChanges: false

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent

        contentHeight: content.height

        VerticalScrollDecorator { }

        Column {
            id: content

            width: parent.width

            PageHeader {
                //: Settings item
                //% "Storage"
                title: qsTrId('id_settings_storage')
            }

            SectionHeader {
                //: Section header on Storage Settings page
                //% "Location"
                text: qsTrId('id_location')
            }

            TextField {
                id: outputLocationField

                //: Description of settings item
                //% "Location for storing the recordings"
                label: qsTrId('id_location_description')

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
                    //: Browse button
                    //% "Browse"
                    text: qsTrId('id_do_browse')

                    onClicked: {
                        var dlg = pageStack.push('../dialogs/FilePickerDialog.qml', {
                            directoryPicker: true,
                            path: outputLocationField.text
                        });

                        dlg.accepted.connect(function() {
                            outputLocationField.text = dlg.absoluteFilePath;
                        })
                    }
                }

                Button {
                    //: Save action
                    //% "Save"
                    text: qsTrId('id_do_save')

                    enabled: !outputLocationField.errorHighlight &&
                                (settings.outputLocation !== outputLocationField.text)

                    onClicked: {
                        var oldLocation = settings.outputLocation;
                        var newLocation = outputLocationField.text;

                        settings.outputLocation = newLocation;

                        if (!fileSystemHelper.dirIsEmpty(oldLocation))
                        {
                            //: Message when changing output location
                            //% "Relocating files"
                            remorse.execute(qsTrId('id_relocating_files'), function() {
                                fileSystemHelper.relocate(oldLocation, newLocation);
                            });
                        }
                    }
                }
            }

            ProgressBar {
                //: Message when changing output location
                //% "Relocating files"
                label: qsTrId('id_relocating_files')

                width: parent.width

                minimumValue: 0

                visible: fileSystemHelper.busy
                value: fileSystemHelper.progress
                maximumValue: fileSystemHelper.totalCount
            }

            SectionHeader {
                //: Storage settings entry
                //% "Storage limits"
                text: qsTrId('id_storage_limits')

                anchors.rightMargin: Theme.paddingLarge
            }

            TextSwitch {
                //: Switch control whether limit storage or not
                //% "Limit storage by size or age"
                text: qsTrId('id_do_limit_storage')

                checked: settings.limitStorage

                onCheckedChanged: {
                    settings.limitStorage = checked;
                }
            }

            ComboBox {
                id: storageAgeCombo

                //: Limit type
                //% "By age"
                label: qsTrId('id_limit_by_age')

                visible: settings.limitStorage

                opacity: visible? 1: 0

                Behavior on opacity {
                    FadeAnimation { }
                }

                menu: ContextMenu {
                    MenuItem {
                        property int value: 0

                        //: Limit type
                        //% "no limit"
                        text: qsTrId('id_limit_none')
                    }

                    MenuItem {
                        property int value: 30

                        //: Limit type
                        //% "30 days"
                        text: qsTrId('id_limit_days_30')
                    }

                    MenuItem {
                        property int value: 90

                        //: Limit type
                        //% "90 days"
                        text: qsTrId('id_limit_days_90')
                    }

                    MenuItem {
                        property int value: 180

                        //: Limit type
                        //% "180 days"
                        text: qsTrId('id_limit_days_180')
                    }

                    MenuItem {
                        property int value: 365

                        //: Limit type
                        //% "365 days"
                        text: qsTrId('id_limit_days_365')
                    }

                    MenuItem {
                        property int value: -1

                        //: Limit type
                        //% "custom"
                        text: qsTrId('id_limit_custom')
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
                //: Text for custom age limit field
                //% "Custom age limit in days"
                label: qsTrId('id_custom_age_limit_description')

                //: Text for custom age limit field
                //% "Custom age limit in days"
                placeholderText: qsTrId('id_custom_age_limit_description')

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

                //: Limit type
                //% "By size"
                label: qsTrId('id_limit_by_size')

                visible: settings.limitStorage

                opacity: visible? 1: 0

                Behavior on opacity {
                    FadeAnimation { }
                }

                menu: ContextMenu {
                    MenuItem {
                        property int value: 0

                        //: Limit type
                        //% "no limit"
                        text: qsTrId('id_limit_none')
                    }

                    MenuItem {
                        property int value: 300

                        //: Limit type
                        //% "300 MB"
                        text: qsTrId('id_limit_size_300')
                    }

                    MenuItem {
                        property int value: 500

                        //: Limit type
                        //% "500 MB"
                        text: qsTrId('id_limit_size_500')
                    }

                    MenuItem {
                        property int value: 1024

                        //: Limit type
                        //% "1 GB"
                        text: qsTrId('id_limit_size_1')
                    }

                    MenuItem {
                        property int value: 3072

                        //: Limit type
                        //% "3 GB"
                        text: qsTrId('id_limit_size_3')
                    }

                    MenuItem {
                        property int value: 5120

                        //: Limit type
                        //% "5 GB"
                        text: qsTrId('id_limit_size_5')
                    }

                    MenuItem {
                        property int value: -1

                        //: Limit type
                        //% "custom"
                        text: qsTrId('id_limit_custom')
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
                //: Text for custom size limit field
                //% "Custom size limit in MB"
                label: qsTrId('id_custom_size_limit_description')

                placeholderText: qsTrId('id_custom_size_limit_description')

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

            SectionHeader {
                //: Settings item on Storage Settings page
                //% "Approval for storage"
                text: qsTrId('id_storage_approval')
            }

            TextSwitch {
                //: Switch control whether to ask for approval for storage
                //% "Require approval"
                text: qsTrId('id_do_require_approval')
                //: Description of "Require approval" switch
                //% "If checked, an approval of storage will be shown after each recorded call"
                description: qsTrId('id_approval_description')

                checked: settings.requireApproval

                onCheckedChanged: {
                    settings.requireApproval = checked;
                }
            }

            Item {
                width: parent.width
                height: Theme.paddingLarge
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
