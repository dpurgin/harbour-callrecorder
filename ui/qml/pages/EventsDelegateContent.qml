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

Row {
    width: parent.width

    Image {
        id: icon

        source: {
            if (model.EventTypeID === 1)
                return 'image://theme/icon-m-incoming-call';
            else if (model.EventTypeID === 3)
                return 'qrc:/images/icon-m-partial-call.png';

            return 'qrc:/images/icon-m-empty.png';
        }
    }

    Column {
        width: parent.width - icon.width - timeStampContainer.width

        Row {
            id: otherPartyId

            spacing: Theme.paddingSmall

            width: parent.width

            Label {
                text: filtered && filters['phoneNumber']?
                          Theme.highlightText(model.PhoneNumberIDRepresentation,
                                              filters['phoneNumber'],
                                              Theme.highlightColor):
                          model.PhoneNumberIDRepresentation;

                visible: person === null

                truncationMode: TruncationMode.Fade

                color: highlight? Theme.highlightColor: Theme.primaryColor
            }

            Label {
                id: primaryName

                text: (person && person.primaryName.length > 0)? person.primaryName: ''

                visible: (person && person.primaryName.length > 0)

                truncationMode: TruncationMode.Fade

                color: highlight? Theme.highlightColor: Theme.primaryColor
            }

            Label {
                text: (person && person.secondaryName.length > 0)? person.secondaryName: ''

                width: Math.max(0, parent.width - primaryName.width - Theme.paddingLarge)

                visible: (person && person.secondaryName.length > 0)

                truncationMode: TruncationMode.Fade

                color: highlight? Theme.secondaryHighlightColor: Theme.secondaryColor

            }
        }

        Label {
            id: description

            width: parent.width

            text: {
                var result = '';

                if (model.RecordingStateID === 1)
                    result = qsTr('Armed for recording')
                else if (model.RecordingStateID === 2)
                    result = qsTr('Recording in progress')
                else if (model.RecordingStateID === 3)
                    result = qsTr('Recording suspended')
                else if (model.RecordingStateID === 4)
                {
                    result = Format.formatDuration(model.Duration, Formatter.DurationShort) +
                             ' \u2022 ' +
                             Format.formatFileSize(model.FileSize);
                }
                else if (model.RecordingStateID === 5)
                {
                    result = qsTr('Waiting for approval')
                }

                if (person)
                {
                    result += ' \u2022 ';

                    if (filtered && filters['phoneNumber'])
                        result += Theme.highlightText(model.PhoneNumberIDRepresentation,
                                                      filters['phoneNumber'],
                                                      Theme.secondaryHighlightColor);
                    else
                        result += model.PhoneNumberIDRepresentation;
                }

                return result;
            }

            font.pixelSize: Theme.fontSizeExtraSmall

            color: highlight? Theme.secondaryHighlightColor: Theme.secondaryColor

            truncationMode: TruncationMode.Fade
        }
    }

    Column {
        id: timeStampContainer

        height: parent.height

        Label {
            id: timeStampDate

            text: Format.formatDate(model.TimeStamp, Formatter.TimepointRelativeCurrentDay)

            anchors.right: parent.right
            anchors.rightMargin: Theme.paddingLarge

            height: timeStampTime.visible?
                        otherPartyId.height:
                        otherPartyId.height + description.height

            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Theme.fontSizeExtraSmall

            color: highlight? Theme.highlightColor: Theme.primaryColor
        }

        Label {
            id: timeStampTime

            text: Format.formatDate(model.TimeStamp, Formatter.TimeValue)

            anchors.right: parent.right
            anchors.rightMargin: Theme.paddingLarge

            visible: startOfDay(new Date()).valueOf() !== startOfDay(model.TimeStamp).valueOf()

            font.pixelSize: Theme.fontSizeExtraSmall

            horizontalAlignment: Text.AlignRight

            color: highlight? Theme.secondaryHighlightColor: Theme.secondaryColor
        }
    }
}
