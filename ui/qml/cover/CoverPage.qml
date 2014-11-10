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

CoverBackground {
    Label {
        id: label
        anchors.centerIn: parent
        text: qsTr("Call Recorder")
    }

    CoverActionList {
        enabled: !systemdUnit.isActive

        CoverAction {
            iconSource: '/usr/share/harbour-callrecorder/images/icon-cover-start-recorder.png'
            onTriggered: systemdUnit.start();
        }
    }   

    CoverActionList {
        enabled: systemdUnit.isActive

        CoverAction {
            iconSource: '/usr/share/harbour-callrecorder/images/icon-cover-stop-recorder.png'
            onTriggered: systemdUnit.stop();
        }
    }
}


