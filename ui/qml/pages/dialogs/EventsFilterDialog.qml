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

import QtQuick 2.2
import Sailfish.Silica 1.0

Dialog {
    SilicaFlickable {
        anchors.fill: parent

        DialogHeader {
            id: dialogHeader

            acceptText: qsTr('Accept')
            cancelText: qsTr('Cancel')
        }

        Grid {
            anchors.top: dialogHeader.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            columns: 2
            verticalItemAlignment: Grid.AlignVCenter

            Switch {
                id: phoneNumberSwitch
            }

            Row {
                width: parent.width - phoneNumberSwitch.width

                TextField {
                    id: phoneNumber

//                    labelVisible: label.length > 0

                    placeholderText: qsTr('Phone number')

                    width: parent.width - personButton.width

                    inputMethodHints: Qt.ImhDialableCharactersOnly

                    onTextChanged: {
                        phoneNumberSwitch.checked = (text.length > 0);

                        if (text.length > 0)
                        {
                            var person = people.personByPhoneNumber(text);

                            if (person)
                                label = Format._joinNames(person.primaryName, person.secondaryName);
                            else
                                label = '';
                        }
                        else
                            label = '';
                    }
                }

                IconButton {
                    id: personButton

                    icon.source: 'image://theme/icon-m-person'

                    onClicked: {
                        var dlg = pageStack.push(
                                    Qt.resolvedUrl('../dialogs/ContactPickerDialog.qml'), {
                                        multiSelect: false,
                                        acceptOnSelect: true,
                                        selectedPhoneNumber: phoneNumber.text
                                    });

                        dlg.accepted.connect(function() {
                            phoneNumber.text = dlg.selectedPhoneNumber;
                        })
                    }
                }
            }

            Switch {

            }

            ValueButton {
                label: qsTr('On')
                value: Format.formatDate(new Date(), Formatter.DateFull)
            }

            Switch {

            }

            ValueButton {
                label: qsTr('After')
                value: Format.formatDate(new Date(), Formatter.DateFull)
            }

            Switch {

            }

            ValueButton {
                label: qsTr('Before')
                value: Format.formatDate(new Date(), Formatter.DateFull)
            }
        }
    }

//    Column {
//        anchors.fill: parent


//        Row {
//            Switch {

//            }

//        }

//        Row {
//            Switch {

//            }

//            Label {
//                text: 'date'
//            }

//            IconButton {
//                icon.source: 'image://theme/icon-m-clear'
//            }
//        }
//    }
}
