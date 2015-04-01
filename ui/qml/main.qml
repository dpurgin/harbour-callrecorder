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
import "pages"

import kz.dpurgin.nemomobile.contacts 1.0
import org.nemomobile.dbus 2.0

import kz.dpurgin.callrecorder.Settings 1.0

ApplicationWindow {
    initialPage: Component { EventsPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    PeopleModel {
        id: people
        filterType: PeopleModel.FilterAll
        requiredProperty: PeopleModel.PhoneNumberRequired
    }

    function startOfDay(dt)
    {
        var dtObj = new Date(dt);
        dtObj.setHours(0);
        dtObj.setMinutes(0);
        dtObj.setSeconds(0);
        dtObj.setMilliseconds(0);

        return dtObj;
    }    

    Settings {
        id: settings

        onSettingsChanged: {
            console.log('settings changed')

            settings.save();

            dbusAdaptor.emitSignal('SettingsChanged');
        }
    }

    DBusAdaptor {
        id: dbusAdaptor

        service: 'kz.dpurgin.DBus.CallRecorder'
        path: '/UI'
        iface: 'kz.dpurgin.DBus.CallRecorder'
    }

    DBusInterface {
        service: 'kz.dpurgin.DBus.CallRecorder'
        path: '/Daemon'
        iface: 'kz.dpurgin.DBus.CallRecorder'

        signalsEnabled: true

        function recorderStateChanged() {
            eventsModel.refresh()
        }
    }

    DBusInterface {
        id: systemdManager

        service: 'org.freedesktop.systemd1'
        path: '/org/freedesktop/systemd1'
        iface: 'org.freedesktop.systemd1.Manager'

        bus: DBusInterface.SessionBus

        signalsEnabled: true

        property var jobRemovedCallbacks: ({})

        function jobRemoved(jobId, objectPath, unit, result)
        {
            console.log('jobId: ' + jobId + ', unit: ' + unit);

            if (unit == 'harbour-callrecorderd.service')
                systemdUnit.updateState();
        }

        function jobNew(id, job, unit)
        {
            console.log('id: ' + id + ', job: ' + job + ', unit: ' + unit);
        }

        function setJobRemovedCallback(jobId, callback)
        {
            jobRemovedCallbacks[jobId] = callback;
        }

        Component.onCompleted: {
            var args = [{
                type: 's',
                value: 'harbour-callrecorderd.service'
            }];

            systemdManager.typedCall('Subscribe', [], function(result) {
                console.log('Subscribe result: ' + result);

                systemdManager.typedCall('LoadUnit', args, function(result) {
                    console.log('Retrieved unit path: ' + result)
                    systemdUnit.path = result
                });
            });

        }
    }

    DBusInterface {
        id: systemdUnit

        service: 'org.freedesktop.systemd1'
        iface: 'org.freedesktop.systemd1.Unit'

        bus: DBusInterface.SessionBus

        property bool isActive: false
        property bool isEnabled: false

        onPathChanged: {
            updateState();
        }

        function disable()
        {
            console.log('');

            var params = [{
                              type: 'as',
                              value: [ 'harbour-callrecorderd.service' ] // unit to disable
                          }, {
                              type: 'b',                                 // 'disable temporarily' flag
                              value: false                               // we disable persistently
                          }];

            systemdManager.typedCall('DisableUnitFiles', params, function(result) {
                console.log(result);

                systemdManager.typedCall('Reload', [], function() {
                    systemdUnit.updateState();
                });
            });

        }

        function enable()
        {
            console.log('');

            var params = [{
                              type: 'as',
                              value: [ 'harbour-callrecorderd.service' ]   // unit to enable
                          }, {
                              type: 'b',                                   // 'enable temporarily' flag
                              value: false                                 // we enable persistently
                          }, {
                              type: 'b',                                   // 'overwrite symlinks' flag
                              value: false                                 // we don't
                          }];

            systemdManager.typedCall('EnableUnitFiles', params, function(result) {
                console.log(result);

                systemdManager.typedCall('Reload', [], function() {
                    systemdUnit.updateState();
                });
            });
        }

        function start()
        {
            console.log('');

            systemdUnit.typedCall('Start', [{type:'s',value:'replace'}]);
        }

        function stop(callback) {
            console.log('');

            systemdUnit.typedCall('Stop', [{type:'s',value:'replace'}]);
        }

        function updateState()
        {
            isActive = (getProperty('ActiveState') === 'active' &&
                        getProperty('SubState') === 'running');
            isEnabled = (getProperty('UnitFileState') === 'enabled');
        }
    }
}
