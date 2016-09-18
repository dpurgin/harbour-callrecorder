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


import QtQuick 2.0
import Sailfish.Silica 1.0

import "../../widgets"

Dialog
{
    property string fileName

    allowedOrientations: Orientation.All

    Column
    {
        anchors.fill: parent

        DialogHeader
        {
            //: Dialog action
            //% "Overwrite"
            acceptText: qsTrId("id_do_overwrite")
        }

        StyledLabel
        {
            //: Backup file location hint
            //% "Backup will be saved to the following file:"
            text: qsTrId("id_backup_location_hint")
        }

        StyledLabel
        {
            text: fileName

            height: implicitHeight + Theme.paddingLarge

            color: Theme.highlightColor
            font.pixelSize: Theme.fontSizeExtraSmall
        }

        StyledLabel
        {
            //: Confirmation to overwrite existing file
            //% "This file already exists. Do you want to overwrite it?
            text: qsTrId("id_confirm_overwrite")
        }

    }
}
