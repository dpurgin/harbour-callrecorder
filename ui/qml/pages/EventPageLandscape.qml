/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2015  Dmitriy Purgin <dpurgin@gmail.com>

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

Component {
    Column {
        width: parent.width

        Column {
            id: detailsColumn

            width: parent.width

            spacing: Theme.paddingLarge

            Label {
                text: eventItem.PhoneNumberIDRepresentation

                anchors.horizontalCenter: parent.horizontalCenter

                font.pixelSize: Theme.fontSizeExtraLarge

                color: Theme.highlightColor

            }

            Label {
                text: person? Format._joinNames(person.primaryName, person.secondaryName): ''

                visible: person != null

                anchors.horizontalCenter: parent.horizontalCenter

                font.pixelSize: Theme.fontSizeSmall

                color: Theme.secondaryHighlightColor
            }

//            Item {
//                width: parent.width
//                height: Theme.paddingLarge * 2
//            }

            Label {
                text: Format.formatDate(eventItem.TimeStamp, Formatter.CallTimeRelative)

                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                text: Format.formatDuration(eventItem.Duration,
                                            eventItem.Duration >= 3600?
                                                Formatter.DurationLong:
                                                Formatter.DurationShort) +
                      ' \u2022 ' + Format.formatFileSize(eventItem.FileSize)

                anchors.horizontalCenter: parent.horizontalCenter

                font.pixelSize: Theme.fontSizeTiny
            }
        }

        Item {
            id: spacer

            width: parent.width
            height: eventPage.height -
                        pageHeader.height -
                        detailsColumn.height -
                        (mediaPlayerControls.visible? mediaPlayerControls.height: 0) -
                        (errorMessageLabel.visible? errorMessageLabel.height: 0) -
                        Theme.paddingLarge
        }

        Row {
            id: mediaPlayerControls

            width: parent.width

            visible: mediaPlayer.error === MediaPlayer.NoError

            Slider {
                id: slider

                property bool mediaPlayerChange: false

                anchors.verticalCenter: parent.verticalCenter

                width: parent.width - playButton.width - Theme.paddingLarge * 2

                maximumValue: mediaPlayer.duration

                valueText: Format.formatDuration(value / 1000, Formatter.DurationShort);

                onValueChanged: {
                    if (!mediaPlayerChange)
                        mediaPlayer.seek(value);
                    else
                        mediaPlayerChange = false;
                }
            }

            IconButton {
                id: playButton

                anchors.verticalCenter: parent.verticalCenter

                icon.source: mediaPlayer.playbackState == MediaPlayer.PlayingState?
                                 'image://theme/icon-l-pause':
                                 'image://theme/icon-l-play'

                onClicked: mediaPlayer.playbackState == MediaPlayer.PlayingState?
                               mediaPlayer.pause():
                               mediaPlayer.play()
            }
        }

        Label {
            id: errorMessageLabel

            width: parent.width

            anchors.horizontalCenter: parent.horizontalCenter

            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter

            visible: mediaPlayer.error !== MediaPlayer.NoError

            text: mediaPlayer.errorString
        }

        Connections {
            target: mediaPlayer

            onPositionChanged: {
                slider.mediaPlayerChange = true
                slider.value = mediaPlayer.position
            }
        }

        Component.onCompleted: {
            slider.mediaPlayerChange = true;
            slider.value = mediaPlayer.position;
        }
    }
}
