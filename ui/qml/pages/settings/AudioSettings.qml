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
    property bool acceptChanges: false // this is set to true when the page is complete, any change
                                       // is made by the user

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content

            anchors.fill: parent

            PageHeader {
                id: pageHeader

                title: qsTr('Audio Settings')
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

            Slider {
                id: compressionSlider

                width: parent.width

                minimumValue: 0
                maximumValue: 8
                stepSize: 1

                label: qsTr('FLAC compression level')

                value: settings.compression

                onValueChanged: {
                    if (acceptChanges)
                        settings.compression = value;
                }
            }
        }
    }

    Component.onCompleted: {
        sampleRateMenu._foreachMenuItem(function(item, index) {
            if (item.value == settings.sampleRate)
            {
                sampleRateCombo.currentIndex = index;
                return false;
            }

            return true;
        });

        acceptChanges = true
    }
}

