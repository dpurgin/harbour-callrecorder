/*
    Call Recorder for SailfishOS
    Copyright (C) 2014  Dmitriy Purgin <dpurgin@gmail.com>

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


Page {
    id: eventsPage

    SilicaListView {
        id: eventsView

        width: parent.width
        height: parent.height

        header: PageHeader {
            title: qsTr("Events")
        }

        VerticalScrollDecorator {}

        model: eventsModel

        delegate: ListItem {
            id: delegate

            property Person person: people.populated? people.personByPhoneNumber(model.PhoneNumberIDRepresentation): null

            width: parent.width

            anchors {
                left: parent.left

                right: parent.right
            }

            Image {
                id: icon

                anchors {
                    left: parent.left
                }

                visible: model.EventTypeID == 1

                source: 'image://theme/icon-m-incoming-call'
            }

            Row {
                id: otherPartyId

                spacing: Theme.paddingSmall
                width: parent.width - icon.width - timeStampDate.width - Theme.paddingLarge

                anchors {
                    left: icon.right
                }

                Label {
                    id: primaryName

                    text: person && person.primaryName.length > 0? person.primaryName : model.PhoneNumberIDRepresentation

                    truncationMode: TruncationMode.Fade
                    color: highlighted? Theme.highlightColor: Theme.primaryColor
                }

                Label {
                    id: secondaryName

                    text: person && person.primaryName.length > 0 && person.secondaryName.length > 0?
                              person.secondaryName :
                              ''

                    truncationMode: TruncationMode.Fade
                    color: highlighted? Theme.secondaryHighlightColor: Theme.secondaryColor

                    width: parent.width - primaryName.width
                }
            }

            Label {
                id: timeStampDate

                anchors {
                    right: parent.right
                    rightMargin: Theme.paddingLarge
                }

                height: timeStampTime.visible? otherPartyId.height: otherPartyId.height + description.height

                verticalAlignment: Text.AlignVCenter
                font.pixelSize: Theme.fontSizeExtraSmall
                color: highlighted? Theme.highlightColor : Theme.primaryColor

                text: Format.formatDate(model.TimeStamp, Formatter.TimepointRelativeCurrentDay)
            }

            Label {
                id: timeStampTime

                anchors {
                    left: timeStampDate.left
                    right: timeStampDate.right
                    top: timeStampDate.bottom
                }

                visible: startOfDay(new Date()).valueOf() != startOfDay(model.TimeStamp).valueOf()

                font.pixelSize: Theme.fontSizeExtraSmall
                horizontalAlignment: Text.AlignRight
                color: highlighted? Theme.secondaryHighlightColor: Theme.secondaryColor

                text: Format.formatDate(model.TimeStamp, Formatter.TimeValue)
            }

            Label {
                id: description

                anchors {
                    top: otherPartyId.bottom
                    left: icon.right
                }

                font.pixelSize: Theme.fontSizeExtraSmall
                text: {
                    switch (model.RecordingStateID)
                    {
                    case 1:
                        return qsTr('Armed for recording')
                    case 2:
                        return qsTr('Recording in progress')
                    case 3:
                        return qsTr('Recording suspended')
                    case 4:
                        return Format.formatDuration(model.Duration, Formatter.DurationShort) + ' \u2022 ' +
                                Format.formatFileSize(model.FileSize);
                    }
                }
                color: highlighted? Theme.secondaryHighlightColor: Theme.secondaryColor
            }

            menu: Component {
                ContextMenu {
                    MenuItem {
                        text: qsTr('Delete')
                        onClicked: removeItem()
                    }
                }
            }

            ListView.onAdd: AddAnimation {
                target: delegate
            }
            ListView.onRemove: RemoveAnimation {
                target: delegate
            }

            onClicked: {
                if (model.RecordingStateID == 4)
                {
                    pageStack.push(Qt.resolvedUrl('EventPage.qml'), {
                        timeStamp: model.TimeStamp,
                        lineIdentification: model.PhoneNumberIDRepresentation,
                        eventTypeId: model.EventTypeID,
                        fileName: model.FileName,
                        fileSize: model.FileSize,
                        duration: model.Duration
                    })
                }
            }

            function removeItem()
            {
               remorseAction(qsTr('Deleting'), function() {
                   eventsModel.removeItem(model.ID, model.FileName);
               })
            }
        }

        ViewPlaceholder {
            id: eventsViewPlaceholder

            text: qsTr("No calls recorded yet")
            enabled: eventsModel.count == 0
        }

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl('AboutPage.qml'))
                }
            }
        }
    }
}


