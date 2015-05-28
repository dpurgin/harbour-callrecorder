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

MouseArea {
    id: button

    property alias iconSource: icon.source
    property alias text: label.text

    property bool highlight: button.pressed || selectedItem === button

    height: content.height + Theme.paddingLarge * 2

    Rectangle {
        anchors.fill: parent

        visible: highlight

        gradient: Gradient {
            GradientStop {
                position: 0
                color: Theme.rgba(Theme.highlightDimmerColor, 0)
            }

            GradientStop {
                position: 1
                color: Theme.rgba(Theme.highlightDimmerColor, 0.2)
            }
        }
    }

    Column {
        id: content

        width: parent.width

        Image {
            id: icon

            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: label

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: Theme.paddingSmall
            anchors.rightMargin: Theme.paddingSmall

            font.pixelSize: Theme.fontSizeExtraSmall

            horizontalAlignment: Text.AlignHCenter

            color: Theme.rgba("black", 0.4)
        }
    }
}
