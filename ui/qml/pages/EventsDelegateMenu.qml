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

import QtQuick 2.2
import Sailfish.Silica 1.0

import kz.dpurgin.callrecorder.Settings 1.0

Component {
    ContextMenu {
        property bool whiteListed: settings.operationMode === Settings.WhiteList &&
                                       whiteListModel.contains(model.PhoneNumberID)
        property bool blackListed: settings.operationMode === Settings.BlackList &&
                                       blackListModel.contains(model.PhoneNumberID)

        MenuItem {
            //% "Approve and store"
            text: qsTrId('id_do_approve_store')

            visible: model.RecordingStateID === 5

            onClicked: maybeApproveItem()
        }

        MenuItem {
            //% "Delete"
            text: qsTrId('id_do_delete')

            onClicked: maybeRemoveItem()
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
                            model.PhoneNumberIDRepresentation);

                if (settings.operationMode === Settings.WhiteList)
                    maybeAddToList(whiteListModel, model.PhoneNumberID, remorseText)
                else if (settings.operationMode === Settings.BlackList)
                    maybeRemoveFromList(blackListModel, model.PhoneNumberID, remorseText);
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
                            model.PhoneNumberIDRepresentation);

                if (settings.operationMode === Settings.BlackList)
                    maybeAddToList(blackListModel, model.PhoneNumberID, remorseText);
                else if (settings.operationMode === Settings.WhiteList)
                    maybeRemoveFromList(whiteListModel, model.PhoneNumberID, remorseText);
            }
        }
    }
}
