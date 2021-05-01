/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2016  Dmitriy Purgin <dpurgin@gmail.com>

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
import org.nemomobile.contacts 1.0

ListItem {
    id: delegate

    property bool selected: selectionMode && isSelected(model.ID)
    readonly property bool highlight: highlighted || selected

    property Person person: people.populated?
                                people.personByPhoneNumber(model.PhoneNumberIDRepresentation):
                                null

    width: parent.width

    _backgroundColor: Theme.rgba(Theme.highlightBackgroundColor,
                                 highlight && !menuOpen?
                                     Theme.highlightBackgroundOpacity:
                                     0)
    menu: EventsDelegateMenu { }

    EventsDelegateContent { }

    onClicked: {
        if (selectionMode)
        {
            if (isSelected(model.ID))
                removeFromSelection(model.ID);
            else
                addToSelection(model.ID);
        }
        else if (model.RecordingStateID === 4 ||
                 model.RecordingStateID === 5)
        {
            pageStack.push(Qt.resolvedUrl('EventPage.qml'), {
                eventItem: model
            });
        }
    }

    Connections {
        target: eventsPage

        onSelectionChanged: {
            selected = isSelected(model.ID)
        }
    }

    function maybeAddToList(listModel, phoneNumberId, remorseText)
    {
        remorseAction(remorseText, function() {
            addToList(listModel, phoneNumberId);
        });
    }

    function maybeApproveItem()
    {
        //% "Storing"
        remorseAction(qsTrId('id_remorse_storing'), function() {
            approveEvent(model.ID);
        });
    }

    function maybeRemoveFromList(listModel, phoneNumberId, remorseText)
    {
        remorseAction(remorseText, function() {
            removeFromList(listModel, phoneNumberId);
        });
    }

    function maybeRemoveItem()
    {
        //% "Deleting"
        remorseAction(qsTrId('id_remorse_deleting'), function() {
            removeEvent(model.ID);
        });
    }
}
