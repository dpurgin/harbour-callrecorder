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

import org.nemomobile.contacts 1.0

import kz.dpurgin.callrecorder.Settings 1.0;

import "../../widgets"

Dialog {
    property int role; // contains BlackList or WhiteList

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                //: Action in phone numbers list
                //% "Pick from contacts"
                text: qsTrId('id_pick_from_contacts')

                onClicked: {
                    var dlg = pageStack.push(Qt.resolvedUrl("ContactPickerDialog.qml"));

                    dlg.accepted.connect(function() {
                        for (var i = 0; i < dlg.selectedPhoneNumbers.length; i++)
                        {
                            var model = phoneNumbersListView.model;
                            var phoneNumberId =
                                    phoneNumbersModel.getIdByLineIdentification(
                                        dlg.selectedPhoneNumbers[i]);

                            if (!model.contains(phoneNumberId))
                                model.add(phoneNumberId);
                        }
                    });
                }
            }

            MenuItem {
                //: Action in phone numbers list
                //% "Copy from white list"
                text: qsTrId('id_copy_from_whitelist')
                visible: role == Settings.BlackList
                enabled: whiteListModel.rowCount > 0

                onClicked: {
                    blackListModel.removeAll();
                    blackListModel.copyFrom('WhiteList')
                }
            }
            MenuItem {
                //: Action in phone numbers list
                //% "Copy from black list"
                text: qsTrId('id_copy_from_blacklist')
                visible: role == Settings.WhiteList
                enabled: blackListModel.rowCount > 0

                onClicked: {
                    whiteListModel.removeAll();
                    whiteListModel.copyFrom('BlackList')
                }
            }
            MenuItem {
                //: Action to remove everything
                //% "Delete all"
                text: qsTrId('id_do_delete_all')
                enabled: phoneNumbersListView.count !== 0

                onClicked: phoneNumbersListView.model.removeAll()
            }
        }

        DialogHeader {
            id: header

            //: Dialog action. Should be equal to the corresponding Sailfish translation
            //% "Save"
            acceptText: qsTrId('id_do_save')
            //: Dialog action. Should be equal to the corresponding Sailfish translation
            //% "Cancel"
            cancelText: qsTrId('id_do_cancel')
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

                        //% "Phone number"
                        placeholderText: qsTrId('id_phone_number')
                        //% "Phone number"
                        label: qsTrId('id_phone_number')

                        Behavior on width {
                            NumberAnimation {}
                        }

//                        onValueChanged: {
//                            phoneNumbersListView.model.setFilterFixedString(value);
//                        }

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
                id: listDelegate

                menu: Component {
                    ContextMenu {
                        MenuItem {
                            //: Delete action
                            //% "Delete"
                            text: qsTrId('id_do_delete')
                            onClicked: phoneNumbersListView.model.removeRow(model.index)
                        }
                    }
                }

                ListView.onAdd: AddAnimation {
                    target: listDelegate
                }
                ListView.onRemove: animateRemoval(listDelegate)
            }            

            ViewPlaceholder {
                enabled: phoneNumbersListView.count == 0

                //: Placeholder for empty list
                //% "No items in the list"
                text: qsTrId('id_no_items')
                //: Hint for empty phone numbers list
                //% "Add numbers with field above or use pull-down menu"
                hintText: qsTrId('id_phone_numbers_hint')
            }
        }
    }

    onAccepted: {
        phoneNumbersListView.model.submit();
    }

    onRejected: {
        phoneNumbersListView.model.revert();
    }
}
