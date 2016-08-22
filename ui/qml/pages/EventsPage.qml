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

import QtQuick 2.0
import Sailfish.Silica 1.0

import kz.dpurgin.nemomobile.contacts 1.0

import kz.dpurgin.callrecorder.Settings 1.0

import "../widgets"

Page {
    id: eventsPage   

    property bool filtered: false
    property var filters: null
    property var filterItems: [ 'phoneNumber', 'onDate', 'beforeDate', 'afterDate' ]

    property bool selectionMode: false
    property bool hasSelection: false
    property var selectedOids: []

    signal selectionChanged()

    allowedOrientations: Orientation.All

    states: [
        State {
            when: !filtered && !selectionMode

            PropertyChanges {
                target: header
                title: qsTr('Recordings')
            }
        },

        State {
            when: !filtered && selectionMode

            PropertyChanges {
                target: header
                title: qsTr('Select recordings')
            }
        },

        State {
            when: filtered && !selectionMode

            PropertyChanges {
                target: header
                title: qsTr('Filtered recordings')
            }
        },

        State {
            when: filtered && selectionMode

            PropertyChanges {
                target: header
                title: qsTr('Select filtered recordings')
            }
        }
    ]

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr('Settings')

                visible: !selectionMode

                onClicked: pageStack.push(Qt.resolvedUrl('SettingsPage.qml'))
            }

            MenuItem {
                text: qsTr('Select recordings')

                visible: !selectionMode
                enabled: eventsModel.rowCount > 0

                onClicked: {
                    pageStack.push(Qt.resolvedUrl('EventsPage.qml'), {
                        selectionMode: true,
                        filtered: filtered,
                        filters: filters
                    })
                }
            }

            MenuItem {
                text: qsTr('Filter')
                enabled: filtered || eventsModel.rowCount > 0
                onClicked: {
                    var config = {};

                    if (filters === null)
                        filters = {};

                    for (var i = 0; i < filterItems.length; i++)
                    {
                        var item = filterItems[i];

                        if (filters[item])
                        {
                            config[item + 'Active'] = true;
                            config[item] = filters[item];
                        }
                    }

                    logObject(config, 'config');

                    var dialog = pageStack.push(Qt.resolvedUrl('dialogs/EventsFilterDialog.qml'),
                                                config)

                    dialog.accepted.connect(function()
                    {
                        filtered = false;
                        filters = {};

                        for (var i = 0; i < filterItems.length; i++)
                        {
                            var item = filterItems[i];

                            if (dialog[item + 'Active'])
                            {
                                filtered = true;
                                filters[item] = dialog[item];
                            }
                        }

                        logObject(filters, 'filters');
                        eventsModel.filter(filters);

                        // If filtering on the Selection page, pass current filters to the main
                        // page, as they share the list model and it is filtered on both pages
                        // anyway. This is needed for correct displaying
                        if (selectionMode)
                        {
                            // Filter dialog is now on top of the page. Underneath it is the
                            // selection page. Main page is the third from the top
                            var mainPage = pageStack.previousPage(pageStack.previousPage());

                            mainPage.filtered = filtered;
                            mainPage.filters = filters;
                        }
                    });
                }
            }

            MenuItem {
                text: filtered?
                          qsTr('Delete all filtered'):
                          qsTr('Delete all')

                visible: selectionMode

                enabled: eventsModel.rowCount > 0

                onClicked: {
                    var remorseText =
                            filtered?
                                qsTr('Deleting all filtered'):
                                qsTr('Deleting all recordings');

                    remorse.execute(remorseText, function() {
                        eventsModel.removeAll();

                        selectedOids = [];
                        selectionChanged();
                    }, 10000);
                }
            }
        }      

        SilicaListView {            
            id: eventsView

            anchors {
                top: parent.top
                bottom: dockedPanel.top
                left: parent.left
                right: parent.right
            }

            clip: true

            VerticalScrollDecorator {}

            model: eventsModel

            header: PageHeader {
                title: qsTr('Recordings')
            }

            delegate: EventsDelegate {
                id: delegate

                ListView.onAdd: AddAnimation {
                    target: delegate
                }
                ListView.onRemove: RemoveAnimation {
                    target: delegate
                }
            }

            ViewPlaceholder {
                id: eventsViewPlaceholder

                text: filtered?
                          qsTr('No recordings meet filter criteria'):
                          qsTr('No calls recorded yet')
                enabled: eventsModel.rowCount === 0
            }
        }

        RemorsePopup {
            id: remorse
        }

        DockedPanel {
            id: dockedPanel

            anchors.left: parent.left
            anchors.right: parent.right

            dock: Dock.Bottom

            height: Theme.itemSizeMedium

            open: selectionMode

            IconButton {
                icon.source: 'image://theme/icon-m-delete'

                anchors.centerIn: parent

                enabled: hasSelection

                onClicked: {
                    var remorseTimeout = (selectedOids.length === eventsModel.rowCount && !filtered?
                                              10000:
                                              5000);

                    remorse.execute(qsTr('Deleting recordings'), function() {
                        if (eventsModel.removeOids(selectedOids))
                        {
                            selectedOids = [];
                            hasSelection = false;
                        }
                    }, remorseTimeout);
                }
            }
        }
    }

    function addToSelection(oid)
    {
        if (selectedOids.indexOf(oid) === -1)
        {
            selectedOids.push(oid);
            selectionChanged()
        }
    }

    function isSelected(oid)
    {
        return selectionMode && selectedOids.indexOf(oid) !== -1;
    }

    function removeFromSelection(oid)
    {
        var idx = selectedOids.indexOf(oid);

        if (idx !== -1)
        {
            selectedOids.splice(idx, 1);
            selectionChanged()
        }
    }

    onSelectionChanged: {
        hasSelection = selectionMode && selectedOids.length > 0;
    }
}
