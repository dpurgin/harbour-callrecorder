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

import kz.dpurgin.callrecorder.LocaleListModel 1.0
import kz.dpurgin.callrecorder.Settings 1.0

Page {
    property bool acceptChanges: false

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent

        PageHeader {
            id: header

            title: qsTr('User interface')
        }

        ComboBox {
            id: localeCombo

            anchors.top: header.bottom

            label: qsTr('Locale')
            value: localeListModel.nativeName(settings.locale)
            description: qsTr('Please restart the application if the locale is changed')

            menu: ContextMenu {
                id: localeMenu

                Repeater {
                    model: localeListModel

                    MenuItem {
                        text: model.nativeName
                        property string value: model.locale
                    }
                }
            }

            onCurrentItemChanged: {
                if (acceptChanges)
                {
                    settings.locale = currentItem.value

                    // restart daemon to pick up new locale
                    if (systemdUnit.isActive)
                        systemdUnit.restart();
                }
            }
        }
    }

    LocaleListModel {
        id: localeListModel
    }

    Component.onCompleted: {
        localeMenu._foreachMenuItem(function(item, index) {
            if (item.value === settings.locale)
            {
                localeCombo.currentIndex = index;
                return false;
            }

            return true;
        });

        acceptChanges = true
    }
}
