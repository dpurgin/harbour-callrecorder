/*
    Call Recorder for SailfishOS
    Copyright (C) 2016 Dmitriy Purgin <dpurgin@gmail.com>

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

import "../../widgets"

Page
{
    allowedOrientations: Orientation.All

    SilicaFlickable
    {
        anchors.fill: parent

        contentHeight: content.height

        VerticalScrollDecorator { }

        Column
        {
            id: content

            width: parent.width

            PageHeader
            {
                title: qsTr('Database Repair')
            }

            StyledLabel
            {
                text: qsTr('Performing database repair. Please do not go back or close the application until the operation is complete')

                height: implicitHeight + Theme.paddingLarge

                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeExtraSmall
            }

            ProgressBar
            {
                width: parent.width

                indeterminate: true

                label: qsTr('Working...')
            }
        }
    }
}
