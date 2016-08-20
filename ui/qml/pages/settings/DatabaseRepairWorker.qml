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

import QtQuick 2.2
import Sailfish.Silica 1.0

import kz.dpurgin.callrecorder.DatabaseRepairWorker 1.0

import "../../widgets"

Page
{
    property int recordRepairMode: DatabaseRepairWorker.Remove
    property int fileRepairMode: DatabaseRepairWorker.Remove

    allowedOrientations: Orientation.All

    DatabaseRepairWorker
    {
        id: worker
    }

    SilicaFlickable
    {
        anchors.fill: parent

        contentHeight: content.height

        VerticalScrollDecorator { }

        Column
        {
            id: content

            width: parent.width

            PageHeader
            {
                title: qsTr('Database Repair')
            }

            StyledLabel
            {
                text: qsTr('Performing database repair. Please do not go back or close the application until the operation is complete')

                height: implicitHeight + Theme.paddingLarge

                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeExtraSmall
            }

            ProgressBar
            {
                width: parent.width

                indeterminate: worker.totalCount < 0

                minimumValue: 0
                maximumValue: worker.totalCount

                value: worker.progress

                label:
                {
                    if (worker.errorCode === DatabaseRepairWorker.None)
                    {
                        switch (worker.operation)
                        {
                            case DatabaseRepairWorker.Starting: return qsTr('Starting...');
                            case DatabaseRepairWorker.ProcessingOrphanedFiles: return qsTr('Processing orphaned files...');
                            case DatabaseRepairWorker.ProcessingOrphanedRecords: return qsTr('Processing orphaned records...');
                            case DatabaseRepairWorker.Complete: return qsTr('Complete!');
                            default:
                        }
                    }
                    else
                    {
                        switch (worker.errorCode)
                        {
                        case DatabaseRepairWorker.UnableToStart: return qsTr('Unable to start operation!');
                        case DatabaseRepairWorker.UnableToRetrieveOrphanedRecords: return qsTr('Unable to retrieve orphaned records!');
                        case DatabaseRepairWorker.UnableToRemoveOrphanedRecord: return qsTr('Unable to remove orphaned record!');
                        case DatabaseRepairWorker.UnableToRetrieveOrphanedFile: return qsTr('Unable to retrieve orphaned file!');
                        case DatabaseRepairWorker.UnableToRestoreOrphanedFile: return qsTr('Unable to restore orphaned file!');
                        case DatabaseRepairWorker.UnhandledException: return qsTr('Unhanled exception!');
                        default:
                        }
                    }

                    return qsTr('Not started');
                }
            }
        }
    }

    onStatusChanged:
    {
        if (status == PageStatus.Activating)
            worker.repair(recordRepairMode, fileRepairMode);
    }
}
