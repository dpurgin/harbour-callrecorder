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

import QtMultimedia 5.0

import kz.dpurgin.callrecorder.Settings 1.0

import org.nemomobile.contacts 1.0

//import "."

Page {
    id: eventPage

    property var eventItem

    property bool whiteListed
    property bool blackListed

    property Person person:
        people.populated?
            people.personByPhoneNumber(eventItem.PhoneNumberIDRepresentation):
            null

    allowedOrientations: Orientation.All

    MediaPlayer {
        id: mediaPlayer

        source: settings.outputLocation + '/' + eventItem.FileName

        onError: {
            console.log(mediaPlayer.errorString)
        }
    }

    SilicaFlickable {        
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                //: Action for approve and store on Events page
                //% "Approve and store"
                text: qsTrId('id_do_approve_store')

                visible: eventItem.RecordingStateID === 5

                onClicked: maybeApproveItem(eventItem.ID)
            }

            MenuLabel {
                text: {
                    var result = '';

                    if (settings.operationMode === Settings.WhiteList && whiteListed)
                    {
                        //: Text for phone number in white list
                        //% "Number is whitelisted"
                        result = qsTrId('id_number_whitelisted')
                    }
                    else if (settings.operationMode === Settings.BlackList && blackListed)
                    {
                        //: Text for phone number in black list
                        //% "Number is blacklisted"
                        result = qsTrId('id_number_blacklisted')
                    }

                    return result;
                }

                visible: whiteListed || blackListed
            }

            MenuItem {
                //: Action for manipulation black/white list
                //% "Always record this number"
                text: qsTrId('id_do_record_number')

                visible: (settings.operationMode === Settings.WhiteList && !whiteListed) ||
                         (settings.operationMode === Settings.BlackList && blackListed)

                onClicked: {
                    //: Remorse text when manipulationg black/white list
                    //% "Recording %1"
                    var remorseText = qsTrId('id_remorse_recording').arg(
                                eventItem.PhoneNumberIDRepresentation);

                    if (settings.operationMode === Settings.WhiteList)
                        maybeAddToList(whiteListModel, eventItem.PhoneNumberID, remorseText)
                    else if (settings.operationMode === Settings.BlackList)
                        maybeRemoveFromList(blackListModel, eventItem.PhoneNumberID, remorseText);
                }
            }

            MenuItem {
                //: Action for manipulation black/white list
                //% "Never record this number"
                text: qsTrId('id_do_not_record_number')

                visible: (settings.operationMode === Settings.BlackList && !blackListed) ||
                         (settings.operationMode === Settings.WhiteList && whiteListed)

                onClicked: {
                    //: Remorse text when manipulationg black/white list
                    //% "Not recording %1"
                    var remorseText = qsTrId('id_remorse_not_recording').arg(
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
                //: Action for deletion
                //% "Delete"
                text: qsTrId('id_do_delete')

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

                title: {
                    if (eventItem.EventTypeID === 1)
                    {
                        //: Call type
                        //% "Incoming call"
                        return qsTrId('id_incoming_call');
                    }
                    else if (eventItem.EventTypeID === 2)
                    {
                        //: Call type
                        //% "Outgoing call"
                        return qsTrId('id_outgoing_call');
                    }
                    else
                    {
                        //: Call type
                        //% "Partial call"
                        return qsTrId('id_partial_call');
                    }
                }
            }

            Loader {
                id: contentLoader

                width: parent.width

                sourceComponent: isPortrait? eventPagePortrait: eventPageLandscape
            }
        }
    }

    RemorsePopup {
        id: remorse
    }

    function maybeApproveItem()
    {
        //: Remorse text when approving recorded call
        //% "Storing"
        remorse.execute(qsTrId('id_remorse_storing'), function() {
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
        //: Remorse text when removing recorded cal
        //% "Deleting"
        remorse.execute(qsTrId('id_remorse_deleting'), function() {
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

    EventPagePortrait {
        id: eventPagePortrait
    }

    EventPageLandscape {
        id: eventPageLandscape
    }
}





