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
    property alias phoneNumberActive: phoneNumberSwitch.checked
    property alias phoneNumber: phoneNumberField.text

    property alias onDateActive: onDateSwitch.checked
    property date onDate: startOfDay(new Date())

    property alias afterDateActive: afterDateSwitch.checked
    property date afterDate: startOfDay(new Date())

    property alias beforeDateActive: beforeDateSwitch.checked
    property date beforeDate: startOfDay(new Date())

    allowedOrientations: Orientation.All

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
                    id: phoneNumberField

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
                                        selectedPhoneNumber: phoneNumberField.text
                                    });

                        dlg.accepted.connect(function() {
                            phoneNumberField.text = dlg.selectedPhoneNumber;
                        })
                    }
                }
            }

            Switch {
                id: onDateSwitch
            }

            ValueButton {
                label: qsTr('On')
                value: Format.formatDate(onDate, Formatter.DateFull)

                onClicked: {
                    var config = {
                        target: 'on',
                        date: onDate
                    };

                    pageStack.push(datePickerDialog, config);
                }
            }

            Switch {
                id: afterDateSwitch
            }

            ValueButton {
                label: qsTr('After')
                value: Format.formatDate(afterDate, Formatter.DateFull)

                onClicked: {
                    var config = {
                        target: 'after',
                        date: afterDate
                    };

                    pageStack.push(datePickerDialog, config);
                }
            }

            Switch {
                id: beforeDateSwitch
            }

            ValueButton {
                label: qsTr('Before')
                value: Format.formatDate(beforeDate, Formatter.DateFull)

                onClicked: {
                    var config = {
                        target: 'before',
                        date: beforeDate
                    }

                    pageStack.push(datePickerDialog, config);
                }
            }
        }
    }

    Component {
        id: datePickerDialog

        DatePickerDialog {
            property var target: null

            onAccepted: {
                var value = startOfDay(date);

                if (target === 'on')
                {
                    onDate = value;
                    onDateActive = true;
                }
                else if (target === 'after')
                {
                    afterDate = value;
                    afterDateActive = true;
                }
                else if (target === 'before')
                {
                    beforeDate = value;
                    beforeDateActive = true;
                }
            }
        }
    }

    onAfterDateActiveChanged: {
        if (afterDateActive && onDateActive)
            onDateActive = false;
    }

    onBeforeDateActiveChanged: {
        if (beforeDateActive && onDateActive)
            onDateActive = false
    }

    onOnDateActiveChanged: {
        if (onDateActive)
        {
            if (afterDateActive)
                afterDateActive = false;

            if (beforeDateActive)
                beforeDateActive = false;
        }
    }
}
