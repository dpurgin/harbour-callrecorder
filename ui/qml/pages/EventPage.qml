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

    property string fileName

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
        anchors.fill: parent

        Column {
            id: mediaPlayerControls

            width: parent.width

            visible: mediaPlayer.error == MediaPlayer.NoError

            Slider {
                id: slider

                property bool mediaPlayerChange: false

                width: parent.width

                handleVisible: true

                maximumValue: mediaPlayer.duration

                onValueChanged: {
                    if (!mediaPlayerChange)
                    {
                        mediaPlayer.seek(value);
                    }
                    else
                    {
                        mediaPlayerChange = false;

                        valueText = Format.formatDuration(value / 1000, Formatter.DurationShort);
                    }
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

            visible: mediaPlayer.error != MediaPlayer.NoError

            text: mediaPlayer.errorString
        }

    }
}





