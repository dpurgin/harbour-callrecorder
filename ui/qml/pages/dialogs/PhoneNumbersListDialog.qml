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

import kz.dpurgin.nemomobile.contacts 1.0

import kz.dpurgin.callrecorder.Settings 1.0;

import "../../widgets"

Dialog {
    property int role; // contains BlackList or WhiteList

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr('Copy from white list')
                visible: role == Settings.BlackList
                enabled: whiteListModel.count > 0
            }
            MenuItem {
                text: qsTr('Copy from black list')
                visible: role == Settings.WhiteList
                enabled: blackListModel.count > 0
            }
            MenuItem {
                text: qsTr('Delete all')
//                enabled: phoneNumbersListView.count !== 0
            }
        }

        DialogHeader {
            id: header

            acceptText: qsTr('Save')
        }

        SilicaListView {
            id: phoneNumbersListView

            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            model: role == Settings.BlackList? blackListModel: whiteListModel;

            header: Item {
                width: parent.width
                height: content.height

                Row {
                    id: content

                    width: parent.width

                    PhoneNumberEntryField {
                        id: inputField

                        width: parent.width - (addButton.visible? addButton.width: 0)

                        placeholderText: qsTr('Search by name or number')

                        Behavior on width {
                            NumberAnimation {}
                        }

                    }

                    IconButton {
                        id: addButton

                        icon.source: 'image://theme/icon-m-add'

                        visible: inputField.value.length > 0 &&
                                    !phoneNumbersListView.model.contains(inputField.value)

                        onClicked: {
                            phoneNumbersListView.model.add(
                                phoneNumbersModel.getIdByLineIdentification(inputField.value))

                            inputField.value = ''
                        }
                    }
                }
            }

            delegate: PhoneNumbersListDelegate {
                menu: Component {
                    ContextMenu {
                        MenuItem {
                            text: qsTr('Remove')
                            onClicked: removeItem()
                        }
                    }
                }

                function removeItem()
                {
                    remorseAction(qsTr('Removing'), function() {
                        phoneNumbersListView.model.removeRow(model.index)
                    })
                }
            }

            ViewPlaceholder {
                enabled: phoneNumbersListView.count == 0

                text: qsTr('No items in the list')
                hintText: qsTr('Add numbers with field above or use pull-down menu')
            }
        }
    }

    onAccepted: {
        phoneNumbersListView.model.submit();
    }

    onRejected: {
        phoneNumbersListView.model.revert();
    }

    Component.onCompleted: {
    }
}
