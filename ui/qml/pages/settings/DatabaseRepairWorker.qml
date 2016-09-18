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

    backNavigation: helper.operation === DatabaseRepairHelper.NotStarted ||
                    helper.operation === DatabaseRepairHelper.Complete

    DatabaseRepairHelper
    {
        id: helper

        onOperationChanged:
        {
            if (operation == DatabaseRepairHelper.Complete)
                eventsModel.refresh();
        }
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
                //: Page header
                //% "Database Repair"
                title: qsTrId("id_database_repair")
            }

            StyledLabel
            {
                //: Ongoing database repair description
                //% "Performing database repair. Please do not close the application until the operation is complete."
                text: qsTrId("id_performing_repair_description")

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
                            //: Database repair progress
                            //% "Starting..."
                            case DatabaseRepairHelper.Starting: return qsTrId("id_starting");
                            //: Database repair progress
                            //% "Processing orphaned files..."
                            case DatabaseRepairHelper.ProcessingOrphanedFiles: return qsTrId("id_processing_orphaned_files");
                            //: Database repair progress
                            //% "Processing orphaned records..."
                            case DatabaseRepairHelper.ProcessingOrphanedRecords: return qsTrId("id_processing_orphaned_records");
                            //: Database repair progress
                            //% "Complete!"
                            case DatabaseRepairHelper.Complete: return qsTrId('id_complete');
                            default:
                        }
                    }
                    else
                    {
                        switch (helper.errorCode)
                        {
                            //: Database repair error description
                            //% "Unable to start operation"
                            case DatabaseRepairHelper.UnableToStart: return qsTrId("id_unable_to_start_operation");
                            //: Database repair error description
                            //% "Unable to retrieve orphaned records"
                            case DatabaseRepairHelper.UnableToRetrieveOrphanedRecords: return qsTrId("id_unable_to_retrieve_orphaned_records");
                            //: Database repair error description
                            //% "Unable to remove orphaned record"
                            case DatabaseRepairHelper.UnableToRemoveOrphanedRecord: return qsTrId("id_unable_to_remove_orphaned_record");
                            //: Database repair error description
                            //% "Unable to retrieve orphaned file"
                            case DatabaseRepairHelper.UnableToRetrieveOrphanedFile: return qsTrId("id_unable_to_retrieve_orphaned_file");
                            //: Database repair error description
                            //% "Unable to restore orphaned file"
                            case DatabaseRepairHelper.UnableToRestoreOrphanedFile: return qsTrId("id_unable_to_restore_orphaned_file");
                            //: Database repair error description
                            //% "Unknown error"
                            case DatabaseRepairHelper.UnhandledException: return qsTrId("id_unknown_error");
                            default:
                        }
                    }

                    //% "Not started"
                    return qsTrId("id_not_started");
                }
            }

            Item
            {
                width: parent.width
                height: Theme.paddingLarge
            }

            DetailItem
            {
                //: Count of repaired records
                //% "Repaired records"
                label: qsTrId("id_repaired_records")
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
                //: Count of repaired files
                //% "Repaired files"
                label: qsTrId("id_repaired_files")
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
