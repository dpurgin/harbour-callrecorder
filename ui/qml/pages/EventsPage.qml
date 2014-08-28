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


Page {
    id: eventsPage

    SilicaListView {
        id: eventsView

        width: parent.width
        height: parent.height

        model: dummyModel

        ListModel {
            id: dummyModel

            ListElement {
                dummy: true
            }
        }

        header: PageHeader {
            title: qsTr("Events")
        }

        ViewPlaceholder {
            id: eventsViewPlaceholder

            text: qsTr("No calls recorded yet")
            enabled: true
        }

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                //onClicked: pageStack.push(Qt.resolvedUrl("SecondPage.qml"))
            }
        }

//        // Tell SilicaFlickable the height of its content.
//        contentHeight: column.height

//        // Place our content in a Column.  The PageHeader is always placed at the top
//        // of the page, followed by our content.
//        Column {
//            id: column

//            width: page.width
//            spacing: Theme.paddingLarge
//            PageHeader {
//                title: qsTr("Events")
//            }
//            Label {
//                x: Theme.paddingLarge
//                text: qsTr("Hello Sailors")
//                color: Theme.secondaryHighlightColor
//                font.pixelSize: Theme.fontSizeExtraLarge
//            }
//        }
    }
}


