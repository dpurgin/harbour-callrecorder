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
    property bool acceptChanges: false // this is set to true when the page is complete, any change
                                       // is made by the user

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content

            anchors.fill: parent

            PageHeader {
                id: pageHeader

                //: Settings item
                //% "Audio settings"
                title: qsTrId('id_settings_audio_settings')
            }

            SectionHeader {
                //: Section header on Audio Settings page
                //% "FLAC Encoder"
                text: qsTrId('id_flac_encoder')
            }

            ComboBox {
                id: sampleRateCombo

                //: Settings item on Audio Settings page
                //% "Sample rate"
                label: qsTrId('id_sample_rate')

                menu: ContextMenu {
                    id: sampleRateMenu

                    MenuItem {
                        //: Audio sample rate
                        //% "44.1 kHz"
                        text: qsTrId('id_sample_rate_44')
                        property int value: 44100
                    }
                    MenuItem {
                        //: Audio sample rate
                        //% "32 kHz"
                        text: qsTrId('id_sample_rate_32')
                        property int value: 32000
                    }
                    MenuItem {
                        //: Audio sample rate
                        //% "22.05 kHz"
                        text: qsTr('id_sample_rate_22')
                        property int value: 22050
                    }
                    MenuItem {
                        //: Audio sample rate
                        //% "16 kHz"
                        text: qsTr('id_sample_rate_16')
                        property int value: 16000
                    }
                    MenuItem {
                        //: Audio sample rate
                        //% "11.025 kHz"
                        text: qsTr('id_sample_rate_11')
                        property int value: 11025
                    }
                    MenuItem {
                        //: Audio sample rate
                        //% "8 kHz"
                        text: qsTr('id_sample_rate_8')
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

                // Settings item on Audio Settings page
                //% "FLAC compression level"
                label: qsTrId('id_flac_compression_level')

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
            if (item.value === settings.sampleRate)
            {
                sampleRateCombo.currentIndex = index;
                return false;
            }

            return true;
        });

        acceptChanges = true
    }
}

