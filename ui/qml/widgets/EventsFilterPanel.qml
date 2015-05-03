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

Column {
    id: root

    property ListModel filtersModel: ListModel {}    

    ListItem {
        id: header

        showMenuOnPressAndHold: false

        Label {
            text: qsTr('Search')

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left

            anchors.leftMargin: Theme.paddingLarge

            color: Theme.highlightColor

            font.pixelSize: Theme.fontSizeLarge

            verticalAlignment: Text.AlignVCenter

            Component.onCompleted: console.log('search wxh: ' + width + 'x' + height)
        }

        IconButton {
            id: addButton

            icon.source: 'image://theme/icon-m-add'

            anchors.right: parent.right
            anchors.rightMargin: Theme.paddingLarge

            onClicked: {
                header.showMenu()
            }
        }

        menu: ContextMenu {
            MenuItem {
                text: qsTr("Phone number")

                onClicked: addFilter("phoneNumber")
            }

            MenuItem {
                text: qsTr("On date")

                onClicked: addFilter("onDate")
            }

            MenuItem {
                text: qsTr("Before date")

                onClicked: addFilter("beforeDate")
            }

            MenuItem {
                text: qsTr("After date")

                onClicked: addFilter("afterDate")
            }

            MenuItem {
                text: qsTr("Call type")

                onClicked: addFilter("eventType")
            }

//            MenuItem {
//                text: qsTr("Duration")

//                onClicked: {
//                    addFilter("duration")
//                }
//            }
        }
    }

    Repeater {
        model: filtersModel

        delegate: Row {
            width: root.width

            TextField {
                width: parent.width - removeButton.width - Theme.paddingLarge

                text: model.stringValue

                visible: model.type === "phoneNumber"
            }

            ValueButton {
                width: parent.width - removeButton.width - Theme.paddingLarge

                label: {
                    var result = "";

                    if (type === "onDate")
                        result = qsTr("On");
                    else if (type === "afterDate")
                        result = qsTr("After");
                    else if (type === "beforeDate")
                        result = qsTr("Before");

                    return result;
                }

                value: Format.formatDate(dateValue)

                visible: type === "onDate" || type === "beforeDate" || type === "afterDate"

                onClicked: {
                    var dlg = pageStack.push("Sailfish.Silica.DatePickerDialog");

                    dlg.accepted.connect(function()
                    {
                        value = dlg.dateText;
                    });
                }
            }

            ComboBox {
                width: parent.width - removeButton.width - Theme.paddingLarge

                label: qsTr("Call type")

                currentIndex: model.intValue

                menu: ContextMenu {
                    width: root.width

                    MenuItem {
                        text: qsTr("incoming")
                    }

                    MenuItem {
                        text: qsTr("outgoing")
                    }

                    MenuItem {
                        text: qsTr("partial")
                    }
                }

                visible: model.type === "eventType"
            }

            IconButton {
                id: removeButton

                icon.source: "image://theme/icon-m-remove"

                onClicked: {
                    filtersModel.remove(model.index)
                }
            }
        }

//        delegate: Item {
//            width: root.width

//            TextField {

//                text: model.stringValue
//            }

//            Label {
//                visible: model.type === "date"

//                text: model.dateValue
//            }

//            Label {
//                visible: model.type === "eventType"
//            }

//            Label {
//                visible: model.type === "duration"
//            }


//        }
    }
    
    function addFilter(type)
    {
        var stringValue = '';
        var dateValue = new Date();
        var intValue = 0;

        switch (type)
        {
        case "eventType":
            intValue = 1;
            break;

        case "duration":
            intValue = 60;
            break;
        }

        var item = {
            "type": type,
            "stringValue": stringValue,
            "dateValue": dateValue,
            "intValue": intValue,
        };

        filtersModel.append(item);
    }
}
