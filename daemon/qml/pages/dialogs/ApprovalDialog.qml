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

import kz.dpurgin.nemomobile.contacts 1.0

import "../../widgets" as Widgets

Widgets.SystemWindow {
    id: window

    PeopleModel {
        id: people

        filterType: PeopleModel.FilterAll
        requiredProperty: PeopleModel.PhoneNumberRequired
    }

    property int eventId
    property int eventTypeId
    property date timeStamp
    property int duration
    property int fileSize
    property alias lineIdentification: lineIdentificationLabel.text

    property Item selectedItem: askLaterButton
    property var person:
        people.populated && lineIdentification?
            people.personByPhoneNumber(lineIdentification):
            null

    Rectangle {
        anchors.fill: parent
        color: Theme.rgba(Theme.highlightDimmerColor, 0.8)
    }

    Rectangle {
        id: dialog

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        color: Theme.highlightBackgroundColor

        height: content.height

        Column {
            id: content

            width: parent.width
            y: Theme.paddingLarge

            Label {
                text: {
                    if (eventTypeId == 1)
                        return qsTr('An incoming call was recorded');
                    else if (eventTypeId == 2)
                        return qsTr('An outgoing call was recorded');
                    else
                        return qsTr('A call was recorded');
                }

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Theme.paddingLarge

                horizontalAlignment: Text.AlignHCenter

                font.pixelSize: Theme.fontSizeLarge

                color: Theme.rgba("black", 0.6)

                wrapMode: Text.Wrap
            }

            Item {
                width: parent.width
                height: Theme.paddingLarge * 2
            }

            Label {
                id: lineIdentificationLabel

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Theme.paddingLarge

                horizontalAlignment: Text.AlignHCenter

                font.pixelSize: Theme.fontSizeLarge

                color: Theme.rgba("black", 0.8)
            }

            Label {
                text: person !== null?
                          Format._joinNames(person.primaryName, person.secondaryName):
                          ''

                visible: person !== null

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Theme.paddingLarge

                horizontalAlignment: Text.AlignHCenter

                font.pixelSize: Theme.fontSizeSmall

                color: Theme.rgba("black", 0.8)

                wrapMode: Text.Wrap
            }

            Item {
                width: parent.width
                height: Theme.paddingMedium
            }

            Label {
                text: Format.formatDate(timeStamp, Formatter.CallTimeRelative) + ' \u2022 ' +
                      Format.formatDuration(duration, Formatter.DurationShort) + ' \u2022 ' +
                      Format.formatFileSize(fileSize)

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Theme.paddingLarge

                horizontalAlignment: Text.AlignHCenter

                font.pixelSize: Theme.fontSizeTiny

                color: Theme.rgba("black", 0.6)
            }

            Item {
                width: parent.width
                height: Theme.paddingLarge * 2
            }

            Row {
                property real buttonWidth: parent.width / 3

                width: parent.width

                Widgets.SystemDialogButton {
                    id: storeButton

                    width: parent.buttonWidth

                    text: qsTr('Store')
                    iconSource: 'image://theme/icon-l-check?#000000'

                    onClicked: {
                        console.log('store button clicked')

                        selectedItem = storeButton
                    }
                }

                Widgets.SystemDialogButton {
                    id: removeButton

                    width: parent.buttonWidth

                    text: qsTr('Remove')
                    iconSource: 'image://theme/icon-l-dismiss?#000000'

                    onClicked: {
                        console.log('remove button clicked')

                        selectedItem = removeButton
                    }
                }

                Widgets.SystemDialogButton {
                    id: askLaterButton

                    width: parent.buttonWidth

                    text: qsTr('Ask later')
                    iconSource: 'image://theme/icon-l-clock?#000000'

                    onClicked: {
                        console.log('ask later')

                        selectedItem = askLaterButton
                    }
                }
            }
        }
    }

    MouseArea {
        anchors {
            top: dialog.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        onClicked: {
            console.log('default clicked')
        }
    }
}
