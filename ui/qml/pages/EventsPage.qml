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
import org.nemomobile.contacts 1.0


Page {
    id: eventsPage

    SilicaListView {
        id: eventsView

        width: parent.width
        height: parent.height

        header: PageHeader {
            title: qsTr('Recordings')
        }

        VerticalScrollDecorator {}

        model: eventsModel

        delegate: EventsDelegate {
            id: delegate

            menu: Component {
                ContextMenu {
                    MenuItem {
                        text: qsTr('Delete')
                        onClicked: removeItem()
                    }
                }
            }

            ListView.onAdd: AddAnimation {
                target: delegate
            }
            ListView.onRemove: RemoveAnimation {
                target: delegate
            }

            onClicked: {
                if (model.RecordingStateID == 4)
                {
                    pageStack.push(Qt.resolvedUrl('EventPage.qml'), {
                        timeStamp: model.TimeStamp,
                        lineIdentification: model.PhoneNumberIDRepresentation,
                        eventTypeId: model.EventTypeID,
                        fileName: model.FileName,
                        fileSize: model.FileSize,
                        duration: model.Duration
                    })
                }
            }

            function removeItem()
            {
               remorseAction(qsTr('Deleting'), function() {
                   eventsModel.removeRow(model.index);
               })
            }
        }

        ViewPlaceholder {
            id: eventsViewPlaceholder

            text: qsTr("No calls recorded yet")
            enabled: eventsModel.rowCount === 0
        }

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl('AboutPage.qml'))
                }
            }

//            MenuItem {
//                text: qsTr('Start Unit')
//                onClicked: {
//                    systemdManager.call('RestartUnit', ['harbour-callrecorderd.service', 'replace'])
//                }
//            }

//            MenuItem {
//                text: qsTr('Stop Unit')
//                onClicked: {
//                    systemdManager.call('StopUnit', ['harbour-callrecorderd.service', 'replace'])
//                }
//            }

            MenuItem {
                text: qsTr('Settings')
                onClicked: pageStack.push(Qt.resolvedUrl('SettingsPage.qml'))
            }

            MenuItem {
                text: qsTr('Select recordings')
                enabled: eventsModel.rowCount > 0
                onClicked: {
                    pageStack.push(Qt.resolvedUrl('EventsPicker.qml'))
                }
            }
        }
    }
}


