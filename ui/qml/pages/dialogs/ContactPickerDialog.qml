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

Dialog {
    id: contactPickerDialog

    // holds all selected phone numbers
    property var selectedPhoneNumbers: []

    // holds a phone number in single-selection mode
    property string selectedPhoneNumber: ''

    // multiSelect controls whether user is permitted to select
    // multiple contacts
    property bool multiSelect: true

    // acceptOnSelect allows automatically accept dialog if a phone number
    // is selected in single-select mode
    property bool acceptOnSelect: false

    signal selectionChanged()

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent

        DialogHeader {
            id: header

            //: Dialog button. This should be the same as the corresponding Sailfish string
            //% "Accept"
            acceptText: qsTrId('id_do_accept')
            //: Dialog button. This should be the same as the corresponding Sailfish string
            //% "Cancel"
            cancelText: qsTrId('id_do_cancel')
        }

        SearchField {
            id: contactSearchField

            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }

        SilicaListView {
            id: listView           

            clip: true

            anchors {
                top: contactSearchField.bottom
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            model: PeopleModel {
                id: peopleModel

                filterType: PeopleModel.FilterAll
                requiredProperty: PeopleModel.PhoneNumberRequired                
                filterPattern: contactSearchField.text
            }

            delegate: ContactPickerDelegate { }

            VerticalScrollDecorator { }

            ViewPlaceholder {
                enabled: listView.count === 0
                //: Placeholder in contact picker
                //% "No contacts available"
                text: qsTrId('id_no_contacts_available')
            }
        }
    }

    onSelectionChanged: {
        canAccept = selectedPhoneNumbers.length > 0;

        if (selectedPhoneNumbers.length === 1 && selectedPhoneNumbers[0] !== selectedPhoneNumber)
            selectedPhoneNumber = selectedPhoneNumbers[0];
        else if (selectedPhoneNumbers.length !== 1)
            selectedPhoneNumber = '';

        if (canAccept && !multiSelect && acceptOnSelect)
            accept();
    }

    function addToSelection(phoneNumber)
    {
        if (multiSelect)
        {
            if (selectedPhoneNumbers.indexOf(phoneNumber) === -1)
            {
                selectedPhoneNumbers.push(phoneNumber);
                selectionChanged();
            }
        }
        else
        {
            if (selectedPhoneNumbers.length === 0 ||
                    selectedPhoneNumbers[0] !== phoneNumber)
            {
                selectedPhoneNumbers = [ phoneNumber ];
                selectionChanged();
            }
        }
    }

    function isSelected(phoneNumber)
    {
        return (selectedPhoneNumbers.indexOf(phoneNumber) !== -1);
    }

    function removeFromSelection(phoneNumber)
    {
        var idx = selectedPhoneNumbers.indexOf(phoneNumber);

        if (idx !== -1)
        {
            selectedPhoneNumbers.splice(idx, 1);
            selectionChanged();
        }
    }

    Component.onCompleted: {
        if (selectedPhoneNumber.length > 0)
            selectedPhoneNumbers = [ selectedPhoneNumber ];
    }
}
