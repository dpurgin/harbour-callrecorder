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

import org.nemomobile.contacts 1.0

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
                //: Title for list of recordings
                //% "Recordings"
                title: qsTrId('id_recordings')
            }
        },

        State {
            when: !filtered && selectionMode

            PropertyChanges {
                target: header
                //: Title for recordings selection
                //% "Select recordings"
                title: qsTrId('id_select_recordings')
            }
        },

        State {
            when: filtered && !selectionMode

            PropertyChanges {
                target: header
                //: Title for filtered list of recordings
                //% "Filtered recordings"
                title: qsTrId('id_filtered_recordings')
            }
        },

        State {
            when: filtered && selectionMode

            PropertyChanges {
                target: header
                //: Title for selection in filtered list of recordings
                //% "Select filtered recordings"
                title: qsTrId('id_select_filtered_recordings')
            }
        }
    ]

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                //% "Settings"
                text: qsTrId('id_settings')

                visible: !selectionMode

                onClicked: pageStack.push(Qt.resolvedUrl('SettingsPage.qml'))
            }

            MenuItem {
                //: Menu item
                //% "Select recordings"
                text: qsTrId('id_do_select_recordings')

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
                //: Menu item
                //% "Filter"
                text: qsTrId('id_do_filter')
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
                          //: Menu item
                          //% "Delete all filtered"
                          qsTrId('id_do_delete_all_filtered'):
                          //% "Delete all"
                          qsTrId('id_do_delete_all')

                visible: selectionMode

                enabled: eventsModel.rowCount > 0

                onClicked: {
                    var remorseText =
                            filtered?
                                //: Remorse text when deleting all filtered recordings
                                //% "Deleting all filtered"
                                qsTrId('id_remorse_deleting_all_filtered'):
                                //: Remorse text when deleting all recordings
                                //% "Deleting all recordings"
                                qsTrId('id_remorse_deleting_all_recordings');

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
                id: header

                //% "Recordings"
                title: qsTrId('id_recordings')
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
                          //: Placeholder text for empty filtered list of recordings
                          //% "No recordings meet filter criteria"
                          qsTrId('id_filtered_recordings_placeholder'):
                          //: Placeholder text for empty list of recordings
                          //% "No calls recorded yet"
                          qsTrId('id_recordings_placeholder')
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

                    //: Remorse text for deleting recordings
                    //% "Deleting recordings"
                    remorse.execute(qsTrId('id_remorse_deleting_recordings'), function() {
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
