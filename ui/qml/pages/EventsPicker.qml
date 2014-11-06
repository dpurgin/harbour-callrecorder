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

    property var selectedOids: []
    property bool hasSelection: false

    SilicaListView {
        id: eventsView

        width: parent.width
        height: parent.height

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: dockedPanel.top
        }

        clip: true

        header: PageHeader {
            title: qsTr('Select Recordings')
        }

        VerticalScrollDecorator {}

        model: eventsModel

        delegate: EventsDelegate {
            id: delegate

            property bool selected: hasSelection && (selectedOids.indexOf(model.ID) != -1)

            ListView.onAdd: AddAnimation {
                target: delegate
            }
            ListView.onRemove: RemoveAnimation {
                target: delegate
            }

            _backgroundColor: Theme.rgba(Theme.highlightBackgroundColor,
                                         (highlighted || selected) && !menuOpen?
                                             Theme.highlightBackgroundOpacity:
                                             0)
            onClicked: {
                selected = !selected;

                var idx = selectedOids.indexOf(model.ID);

                if (idx == -1)
                    selectedOids.push(model.ID);
                else
                    selectedOids.splice(idx, 1);

                hasSelection = (selectedOids.length > 0);
            }            
        }

        ViewPlaceholder {
            id: eventsViewPlaceholder

            text: qsTr("No calls recorded yet")
            enabled: eventsModel.rowCount === 0
        }

        PullDownMenu {
            MenuItem {
                text: qsTr('Delete all')
                enabled: eventsModel.rowCount > 0
                onClicked: {
                    remorse.execute(qsTr('Deleting all recordings'), function() {
                        eventsModel.removeAll();
                        hasSelection = false;
                        selectedOids = [];
                    }, 10000);
                }
            }
        }
    }

    RemorsePopup {
        id: remorse
    }

    DockedPanel {
        id: dockedPanel

        width: parent.width
        height: Theme.itemSizeLarge
        dock: Dock.Bottom
        open: hasSelection

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: "image://theme/graphic-gradient-edge"
        }

        IconButton {
            icon.source: "image://theme/icon-m-delete"
            anchors.centerIn: parent

            onClicked: {
                remorse.execute(qsTr('Deleting recordings'), function() {
                    if (eventsModel.removeOids(selectedOids))
                    {
                        selectedOids = [];
                        hasSelection = false;
                    }
                }, selectedOids.length == eventsModel.count? 10000: 5000);
            }
        }
    }
}
