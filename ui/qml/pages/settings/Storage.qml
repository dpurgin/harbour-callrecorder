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
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content

            anchors.fill: parent

            PageHeader {
                id: pageHeader

                title: qsTr('Storage')
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
                maximumValue: fileSystemHelper.totalCount
            }
        }
    }

    RemorsePopup {
        id: remorse
    }
}
