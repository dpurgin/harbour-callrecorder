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

import kz.dpurgin.callrecorder.Settings 1.0

import kz.dpurgin.nemomobile.contacts 1.0

import QtMultimedia 5.0

Page {
    id: eventPage

    property var eventItem

    property bool whiteListed
    property bool blackListed

    property Person person:
        people.populated?
            people.personByPhoneNumber(eventItem.PhoneNumberIDRepresentation):
            null

    MediaPlayer {
        id: mediaPlayer

        source: settings.outputLocation + '/' + eventItem.FileName

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

        PullDownMenu {
            MenuItem {
                text: qsTr('Approve and store')

                visible: eventItem.RecordingStateID === 5

                onClicked: maybeApproveItem(eventItem.ID)
            }

            MenuLabel {
                text: {
                    var result = '';

                    if (settings.operationMode === Settings.WhiteList && whiteListed)
                    {
                        result = qsTr('Number is whitelisted')
                    }
                    else if (settings.operationMode === Settings.BlackList && blackListed)
                    {
                        result = qsTr('Number is blacklisted')
                    }

                    return result;
                }

                visible: whiteListed || blackListed
            }

            MenuItem {
                text: qsTr('Always record this number')

                visible: (settings.operationMode === Settings.WhiteList && !whiteListed) ||
                         (settings.operationMode === Settings.BlackList && blackListed)

                onClicked: {
                    var remorseText = qsTr('Recording %1').arg(
                                eventItem.PhoneNumberIDRepresentation);

                    if (settings.operationMode === Settings.WhiteList)
                        maybeAddToList(whiteListModel, eventItem.PhoneNumberID, remorseText)
                    else if (settings.operationMode === Settings.BlackList)
                        maybeRemoveFromList(blackListModel, eventItem.PhoneNumberID, remorseText);
                }
            }

            MenuItem {
                text: qsTr('Never record this number')

                visible: (settings.operationMode === Settings.BlackList && !blackListed) ||
                         (settings.operationMode === Settings.WhiteList && whiteListed)

                onClicked: {
                    var remorseText = qsTr('Not recording %1').arg(
                                eventItem.PhoneNumberIDRepresentation);

                    if (settings.operationMode === Settings.BlackList)
                        maybeAddToList(blackListModel, eventItem.PhoneNumberID, remorseText);
                    else if (settings.operationMode === Settings.WhiteList)
                        maybeRemoveFromList(whiteListModel, eventItem.PhoneNumberID, remorseText);
                }
            }
        }

        PushUpMenu {
            highlightColor: "#ff8084"
            backgroundColor: "#ff1a22"

            MenuItem {
                text: qsTr('Delete')

                color: "#FF8080"

                onClicked: maybeRemoveItem(eventItem.ID)
            }
        }

        contentHeight: contentColumn.height

        Column {
            id: contentColumn

            width: parent.width

            PageHeader {
                id: pageHeader

                title: qsTr('Details')
            }

            Column {
                id: detailsColumn

                width: parent.width

                spacing: Theme.paddingLarge

                Label {
                    text: {
                        if (eventItem.EventTypeID === 1)
                            return qsTr('Incoming call');
                        else if (eventItem.EventTypeID === 2)
                            return qsTr('Outgoing call');
                        else
                            return qsTr('Partial call');
                    }

                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Item {
                    width: parent.width
                    height: Theme.paddingLarge * 2
                }

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

                Item {
                    width: parent.width
                    height: Theme.paddingLarge * 2
                }

                Label {
                    text: Format.formatDate(eventItem.TimeStamp, Formatter.CallTimeRelative)

                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Label {
                    text: Format.formatDuration(eventItem.Duration, Formatter.DurationShort) +
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
                            Theme.paddingLarge * 2
            }

            Column {
                id: mediaPlayerControls

                width: parent.width

                visible: mediaPlayer.error === MediaPlayer.NoError

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

                IconButton {
                    icon.source: mediaPlayer.playbackState == MediaPlayer.PlayingState?
                                     'image://theme/icon-l-pause':
                                     'image://theme/icon-l-play'

                    anchors.horizontalCenter: parent.horizontalCenter

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
        }        
    }

    RemorsePopup {
        id: remorse
    }

    function maybeApproveItem()
    {
        remorse.execute(qsTr('Storing'), function() {
            approveEvent(eventItem.ID);
        });
    }

    function maybeAddToList(listModel, phoneNumberId, remorseText)
    {
        remorse.execute(remorseText, function() {
            addToList(listModel, phoneNumberId);
        });
    }

    function maybeRemoveItem()
    {
        remorse.execute(qsTr('Deleting'), function() {
            removeEvent(eventItem.ID);
            pageStack.pop();
        });
    }

    function maybeRemoveFromList(listModel, phoneNumberId, remorseText)
    {
        remorse.execute(remorseText, function() {
            removeFromList(listModel, phoneNumberId);
        });
    }

    function updateListProperties()
    {
        whiteListed = (settings.operationMode === Settings.WhiteList &&
                       whiteListModel.contains(eventItem.PhoneNumberID));
        blackListed = (settings.operationMode === Settings.BlackList &&
                       blackListModel.contains(eventItem.PhoneNumberID));
    }

    Connections {
        target: app

        onPhoneNumberListsChanged: updateListProperties()
    }

    Connections {
        target: settings

        onOperationModeChanged: updateListProperties();
    }

    Component.onCompleted: {
        updateListProperties();
    }
}





