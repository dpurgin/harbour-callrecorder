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

import QtMultimedia 5.0

Page {
    id: eventPage

    property string timeStamp
    property string lineIdentification
    property int eventTypeId
    property string fileName
    property int fileSize
    property int duration

    MediaPlayer {
        id: mediaPlayer

        source: StandardPaths.data + '/data/' + fileName

        onPositionChanged: {
            slider.mediaPlayerChange = true
            slider.value = position
        }

        onError: {
            console.log(mediaPlayer.errorString)
        }
    }

    SilicaFlickable {        
        anchors {
            fill: parent

            leftMargin: Theme.paddingLarge
            rightMargin: Theme.paddingLarge
        }

        contentHeight: contentColumn.height

        Column {
            id: contentColumn

            PageHeader {
                id: pageHeader

                title: qsTr("Details")
            }

            width: parent.width

            Column {
                id: detailsColumn

                width: parent.width
                spacing: Theme.paddingLarge

                Label {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }

                    text: eventTypeId == 1? qsTr('Incoming call'): qsTr('Outgoing call')
                }

                Item {
                    width: parent.width
                    height: Theme.paddingLarge * 2
                }

                Label {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }

                    font.pixelSize: Theme.fontSizeExtraLarge

                    color: Theme.highlightColor

                    text: lineIdentification
                }

                Item {
                    width: parent.width
                    height: Theme.paddingLarge * 2
                }

                Label {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }

                    text: Format.formatDate(timeStamp, Formatter.CallTimeRelative)
                }

                Label {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }

                    font.pixelSize: Theme.fontSizeTiny

                    text: Format.formatDuration(duration, Formatter.DurationShort) + ' \u2022 ' + Format.formatFileSize(fileSize)
                }
            }

            Item {
                width: parent.width
                height: eventPage.height - pageHeader.height - detailsColumn.height - Theme.paddingLarge -
                        (mediaPlayer.error == MediaPlayer.NoError?
                             mediaPlayerControls.height:
                             errorMessageLabel.height)
            }

            Column {
                id: mediaPlayerControls

                width: parent.width

                visible: mediaPlayer.error == MediaPlayer.NoError

                Slider {
                    id: slider

                    property bool mediaPlayerChange: false

                    width: parent.width

                    maximumValue: mediaPlayer.duration

                    valueText: Format.formatDuration(value / 1000, Formatter.DurationShort);

                    onValueChanged: {
                        if (!mediaPlayerChange)
                            mediaPlayer.seek(value);
                        else
                            mediaPlayerChange = false;
                    }
                }

                Row {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }

                    IconButton {
                        icon.source: 'image://theme/icon-l-left'
                    }

                    IconButton {
                        icon.source: mediaPlayer.playbackState == MediaPlayer.PlayingState?
                                         'image://theme/icon-l-pause':
                                         'image://theme/icon-l-play'

                        onClicked: mediaPlayer.playbackState == MediaPlayer.PlayingState?
                                       mediaPlayer.pause():
                                       mediaPlayer.play()
                    }

                    IconButton {
                        icon.source: 'image://theme/icon-l-right'
                    }
                }
            }

            Label {
                id: errorMessageLabel

                width: parent.width

                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter

                visible: mediaPlayer.error != MediaPlayer.NoError

                text: mediaPlayer.errorString
            }
        }
    }
}





