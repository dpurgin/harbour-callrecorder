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

import kz.dpurgin.callrecorder.DatabaseRepairHelper 1.0

import "../../widgets"

Page
{
    property int recordRepairMode: DatabaseRepairHelper.Remove
    property int fileRepairMode: DatabaseRepairHelper.Remove

    allowedOrientations: Orientation.All

    DatabaseRepairHelper
    {
        id: helper
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

                indeterminate: helper.totalCount < 0

                minimumValue: 0
                maximumValue: helper.totalCount

                value: helper.progress

                label:
                {
                    if (helper.errorCode === DatabaseRepairHelper.None)
                    {
                        switch (helper.operation)
                        {
                            case DatabaseRepairHelper.Starting: return qsTr('Starting...');
                            case DatabaseRepairHelper.ProcessingOrphanedFiles: return qsTr('Processing orphaned files...');
                            case DatabaseRepairHelper.ProcessingOrphanedRecords: return qsTr('Processing orphaned records...');
                            case DatabaseRepairHelper.Complete: return qsTr('Complete!');
                            default:
                        }
                    }
                    else
                    {
                        switch (helper.errorCode)
                        {
                            case DatabaseRepairHelper.UnableToStart: return qsTr('Unable to start operation!');
                            case DatabaseRepairHelper.UnableToRetrieveOrphanedRecords: return qsTr('Unable to retrieve orphaned records!');
                            case DatabaseRepairHelper.UnableToRemoveOrphanedRecord: return qsTr('Unable to remove orphaned record!');
                            case DatabaseRepairHelper.UnableToRetrieveOrphanedFile: return qsTr('Unable to retrieve orphaned file!');
                            case DatabaseRepairHelper.UnableToRestoreOrphanedFile: return qsTr('Unable to restore orphaned file!');
                            case DatabaseRepairHelper.UnhandledException: return qsTr('Unhanled exception!');
                            default:
                        }
                    }

                    return qsTr('Not started');
                }
            }

            Item
            {
                width: parent.width
                height: Theme.paddingLarge
            }

            DetailItem
            {
                label: qsTr('Repaired records')
                value: helper.repairedRecordsCount

                width: parent.width

                visible: helper.repairedRecordsCount > 0

                opacity: visible? 1: 0

                Behavior on opacity
                {
                    FadeAnimator { }
                }
            }

            DetailItem
            {
                label: qsTr('Repaired files')
                value: helper.repairedFilesCount

                width: parent.width

                visible: helper.repairedFilesCount > 0

                opacity: visible? 1: 0

                Behavior on opacity
                {
                    FadeAnimator { }
                }
            }
        }
    }

    onStatusChanged:
    {
        if (status == PageStatus.Activating)
            helper.repair(recordRepairMode, fileRepairMode);
    }
}
