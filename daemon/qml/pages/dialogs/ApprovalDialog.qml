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

import QtQuick 2.2
import Sailfish.Silica 1.0

import kz.dpurgin.nemomobile.contacts 1.0

import "../../widgets" as Widgets

Item {
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
    property string lineIdentification

    property bool shouldBeVisible

    property Item selectedItem: askLaterButton
    property var person:
        people.populated && lineIdentification?
            people.personByPhoneNumber(lineIdentification):
            null

    signal askLaterClicked(int eventId)
    signal storeClicked(int eventId)
    signal removeClicked(int eventId)

    width: Screen.width
    height: Screen.height

    opacity: shouldBeVisible? 1: 0

    Behavior on opacity {
        SequentialAnimation {
            FadeAnimation { }
            ScriptAction {
                script: {
                    if (!shouldBeVisible && eventId > -1)
                    {
                        if (selectedItem === storeButton)
                            storeClicked(eventId)
                        else if (selectedItem === removeButton)
                            removeClicked(eventId)
                        else if (selectedItem === askLaterButton)
                            askLaterClicked(eventId)
                    }
                }
            }
        }
    }

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

            Item {
                width: parent.width
                height: Theme.paddingLarge
            }

            Label {
                text: {
                    if (eventTypeId == 1)
                        return qsTr('An incoming call was recorded');
                    else if (eventTypeId == 2)
                        return qsTr('An outgoing call was recorded');
                    else
                        return qsTr('A call was recorded');
                }

                x: Theme.horizontalPageMargin
                width: parent.width - 2 * x

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
                text: lineIdentification

                x: Theme.horizontalPageMargin
                width: parent.width - 2 * x

                horizontalAlignment: Text.AlignHCenter

                font.pixelSize: Theme.fontSizeLarge

                color: Theme.rgba("black", 0.8)
            }

            Label {
                text: person !== null?
                          Format._joinNames(person.primaryName, person.secondaryName):
                          ''

                visible: person !== null

                x: Theme.horizontalPageMargin
                width: parent.width - 2 * x

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

                x: Theme.horizontalPageMargin
                width: parent.width - 2 * x

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

                Widgets.ApprovalDialogButton {
                    id: storeButton

                    width: parent.buttonWidth

                    text: qsTr('Store')
                    iconSource: 'image://theme/icon-l-check?#000000'

                    onClicked: {
                        console.log('store clicked');

                        selectedItem = storeButton
                        shouldBeVisible = false
                    }
                }

                Widgets.ApprovalDialogButton {
                    id: removeButton

                    width: parent.buttonWidth

                    text: qsTr('Remove')
                    iconSource: 'image://theme/icon-l-dismiss?#000000'

                    onClicked: {
                        console.log('remove clicked');

                        selectedItem = removeButton
                        shouldBeVisible = false
                    }
                }

                Widgets.ApprovalDialogButton {
                    id: askLaterButton

                    width: parent.buttonWidth

                    text: qsTr('Ask later')
                    iconSource: 'image://theme/icon-l-clock?#000000'

                    onClicked: {
                        console.log('ask later clicked');

                        selectedItem = askLaterButton
                        shouldBeVisible = false
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
            console.log('default area clicked');

            // default to Ask Later for safety
            selectedItem = askLaterButton
            shouldBeVisible = false
        }
    }

    Timer {
        id: timer

        repeat: false

        interval: 30000

        onTriggered: {
            console.log('selection timed out')

            selectedItem = askLaterButton
            shouldBeVisible = false
        }
    }

    onShouldBeVisibleChanged: {
        if (shouldBeVisible)
            timer.restart()
        else
            timer.stop()
    }
}
