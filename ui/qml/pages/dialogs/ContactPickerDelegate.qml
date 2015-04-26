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

ListItem {
    property bool selected: false
    property bool highlight: highlighted || selected

    showMenuOnPressAndHold: false

    _backgroundColor: Theme.rgba(Theme.highlightBackgroundColor,
                                 (highlight) && !menuOpen?
                                     Theme.highlightBackgroundOpacity:
                                     0)

    Row {
        id: container

        width: parent.width
        height: Theme.itemSizeMedium

        Item {
            id: leftSpacer

            width: Theme.paddingLarge
            height: parent.height
        }

        Column {
            width: parent.width - leftSpacer.width - rightSpacer.width
            height: parent.height

            Row {
                width: parent.width
                spacing: Theme.paddingSmall

                Label {
                    text: model.primaryName

                    color: highlight? Theme.highlightColor: Theme.primaryColor
                }

                Label {
                    text: model.secondaryName

                    color: highlight? Theme.secondaryHighlightColor: Theme.secondaryColor

                    wrapMode: Text.NoWrap
                    truncationMode: TruncationMode.Fade
                }
            }

            Label {
                text: {
                    var repr = '';

                    for (var i = 0; i < model.phoneNumbers.length; i++)
                    {
                        if (repr.length > 0)
                            repr += ', '

                        repr += model.phoneNumbers[i];
                    }

                    return repr;
                }

                width: parent.width

                font.pixelSize: Theme.fontSizeExtraSmall

                color: highlight? Theme.secondaryHighlightColor: Theme.secondaryColor

                wrapMode: Text.NoWrap
                truncationMode: TruncationMode.Fade
            }
        }

        Item {
            id: rightSpacer

            width: Theme.paddingLarge
            height: parent.height
        }
    }

    menu: ContactPickerDelegateMenu {
        phoneNumbers: model.phoneNumbers

        onClosed: {
            updateSelected();
        }
    }    

    onClicked: {
        if (model.phoneNumbers.length > 1)
            showMenu()
        else if (model.phoneNumbers.length === 1)
        {
            var phoneNumber = model.phoneNumbers[0];

            if (isSelected(phoneNumber))
                removeFromSelection(phoneNumber);
            else
                addToSelection(phoneNumber);

            updateSelected();
        }
    }

    Component.onCompleted: {
        updateSelected();
    }

    function updateSelected()
    {
        var doSelect = false;

        for (var i = 0; i < model.phoneNumbers.length && !doSelect; i++)
        {
            if (selectedPhoneNumbers.indexOf(model.phoneNumbers[i]) !== -1)
                doSelect = true;
        }

        if (selected !== doSelect)
            selected = doSelect;
    }
}
