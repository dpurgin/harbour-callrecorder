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
    id: settingsPage

    Timer {
        id: dbusTimer
        interval: 500

        onTriggered: {
            checkActiveState();
        }
    }

    SilicaFlickable {
        anchors.fill: parent

        contentHeight: contentColumn.height

        Column {
            id: contentColumn

            width: parent.width

            PageHeader {
                title: qsTr('Settings')
            }

            SectionHeader {
                text: qsTr('Recorder Daemon')
            }

            TextSwitch {
                id: activeSwitch

                automaticCheck: false

                text: qsTr('Active')
                description: qsTr('Capture all incoming and outgoing calls')

                onClicked: {
                    activeSwitch.busy = true

                    var method = activeSwitch.checked? 'Stop': 'Start';

                    systemdUnit.typedCallWithReturn(method, [{type:'s',value:'replace'}], function(result) {
                        console.log(result)

                        dbusTimer.start();
                    })
                }
            }

            TextSwitch {
                id: startupTypeSwitch

                automaticCheck: false

                text: qsTr('Automatic startup')
                description: qsTr('Start automatically upon reboot')

                onClicked: {
                    startupTypeSwitch.busy = true

                    var method = startupTypeSwitch.checked? 'DisableUnitFiles': 'EnableUnitFiles';

                    var params = [];

                    if (!startupTypeSwitch.checked)
                    {
                        params = [{
                                      type: 'as',
                                      value: [ 'harbour-callrecorderd.service' ]   // unit to enable
                                  }, {
                                      type: 'b',                                   // 'enable temporarily' flag
                                      value: false                                 // we enable persistently
                                  }, {
                                      type: 'b',                                   // 'overwrite symlinks' flag
                                      value: false                                 // we don't
                                  }];
                    }
                    else
                    {
                        params = [{
                                      type: 'as',
                                      value: [ 'harbour-callrecorderd.service' ] // unit to disable
                                  }, {
                                      type: 'b',                                 // 'disable temporarily' flag
                                      value: false                               // we disable persistently
                                  }];
                    }

                    systemdManager.typedCallWithReturn(method, params, function(result) {
                        systemdManager.typedCallWithReturn('Reload', [], function() {
                            checkStartupType();
                        });
                    });
                }
            }
        }
    }

    Component.onCompleted: {
        checkActiveState();
        checkStartupType();
    }

    function checkActiveState()
    {
        activeSwitch.busy = true;
        activeSwitch.checked = (systemdUnit.getProperty('ActiveState') === 'active' &&
                                systemdUnit.getProperty('SubState') === 'running');
        activeSwitch.busy = false;
    }

    function checkStartupType()
    {
        startupTypeSwitch.busy = true;

        startupTypeSwitch.checked = (systemdUnit.getProperty('UnitFileState') === 'enabled');

        startupTypeSwitch.busy = false;
    }
}
