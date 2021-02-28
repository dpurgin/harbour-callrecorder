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
import org.nemomobile.contacts 1.0

ListItem {
    property QtObject person: (people.populated?
                                people.personByPhoneNumber(model.PhoneNumberIDRepresentation):
                                null);

    Column {
        x: Theme.paddingLarge

        Row {
            spacing: Theme.paddingSmall

            Label {
                text: person? person.primaryName: model.PhoneNumberIDRepresentation
                color: highlighted? Theme.highlightColor: Theme.primaryColor

                truncationMode: TruncationMode.Fade
            }

            Label {
                text: person? person.secondaryName: ''
                color: highlighted? Theme.secondaryHighlightColor: Theme.secondaryColor

                truncationMode: TruncationMode.Fade
            }
        }

        Label {
            text: person? model.PhoneNumberIDRepresentation: '';

            font.pixelSize: Theme.fontSizeExtraSmall
            color: highlighted? Theme.secondaryHighlightColor: Theme.secondaryColor

            truncationMode: TruncationMode.Fade
        }
    }
}
