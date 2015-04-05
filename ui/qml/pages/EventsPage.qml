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

import kz.dpurgin.nemomobile.contacts 1.0

import kz.dpurgin.callrecorder.Settings 1.0

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
                    property bool whiteListed: settings.operationMode === Settings.WhiteList &&
                                                   whiteListModel.contains(model.PhoneNumberID)
                    property bool blackListed: settings.operationMode === Settings.BlackList &&
                                                   blackListModel.contains(model.PhoneNumberID)

                    MenuItem {
                        text: qsTr('Delete')
                        onClicked: removeItem()
                    }

                    MenuLabel {
                        text: {
                            var result = '';

                            if (settings.operationMode === Settings.WhiteList && whiteListed)
                            {
                                result = qsTr('%1 is whitelisted')
                                            .arg(model.PhoneNumberIDRepresentation)
                            }
                            else if (settings.operationMode === Settings.BlackList && blackListed)
                            {
                                result = qsTr('%1 is blacklisted')
                                            .arg(model.PhoneNumberIDRepresentation)
                            }

                            return result;
                        }

                        visible: whiteListed || blackListed
                    }

                    MenuItem {
                        text: whiteListed || blackListed?
                                  qsTr('Always record this number'):
                                  qsTr('Always record %1').arg(model.PhoneNumberIDRepresentation)

                        visible: (settings.operationMode === Settings.WhiteList && !whiteListed) ||
                                 (settings.operationMode === Settings.BlackList && blackListed)

                        onClicked: {
                            var remorseText = qsTr('Recording %1').arg(
                                        model.PhoneNumberIDRepresentation);

                            if (settings.operationMode === Settings.WhiteList)
                                addToList(whiteListModel, model.PhoneNumberID, remorseText)
                            else if (settings.operationMode === Settings.BlackList)
                                removeFromList(blackListModel, model.PhoneNumberID, remorseText);
                        }
                    }

                    MenuItem {
                        text: whiteListed || blackListed?
                                  qsTr('Never record this number'):
                                  qsTr('Never record %1').arg(model.PhoneNumberIDRepresentation)

                        visible: (settings.operationMode === Settings.BlackList && !blackListed) ||
                                 (settings.operationMode === Settings.WhiteList && whiteListed)

                        onClicked: {
                            var remorseText = qsTr('Not recording %1').arg(
                                        model.PhoneNumberIDRepresentation);

                            if (settings.operationMode === Settings.BlackList)
                                addToList(blackListModel, model.PhoneNumberID, remorseText);
                            else if (settings.operationMode === Settings.WhiteList)
                                removeFromList(whiteListModel, model.PhoneNumberID, remorseText);
                        }
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

            function addToList(listModel, phoneNumberId, remorseText)
            {
                remorseAction(remorseText, function() {
                    if (!listModel.contains(phoneNumberId))
                    {
                        listModel.add(phoneNumberId)
                        listModel.submit();
                    }
                })
            }

            function removeFromList(listModel, phoneNumberId, remorseText)
            {
                remorseAction(remorseText, function() {
                    listModel.remove(phoneNumberId);
                })
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


