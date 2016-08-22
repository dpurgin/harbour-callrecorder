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

import Nemo.FileManager 1.0

import "../../widgets"

Dialog
{
    property bool directoryPicker: false
    property alias path: fileModel.path

    property string absoluteFilePath:
        directoryPicker?
            fileModel.absolutePath:
            (fileModel.selectedCount > 0? fileModel.selectedFiles()[0]: '')

    property string fileName: absoluteFilePath.substring(absoluteFilePath.lastIndexOf('/') + 1)

    allowedOrientations: Orientation.All
    canAccept: absoluteFilePath !== '' &&
                (directoryPicker?
                     fileSystemHelper.isWritable(absoluteFilePath):
                     fileSystemHelper.isReadable(absoluteFilePath))

    FileModel
    {
        id: fileModel
        active: true
        includeDirectories: true
        directorySort: FileModel.SortDirectoriesBeforeFiles
        sortBy: FileModel.SortByName
        sortOrder: Qt.AscendingOrder
        path: '/home/nemo'
    }

    Column
    {
        anchors.fill: parent

        DialogHeader
        {
            id: header

            acceptText: absoluteFilePath !== ''?
                            qsTr('Select %1').arg(fileName):
                            qsTr('Select')
        }

        SilicaListView
        {
            width: parent.width
            height: parent.height - header.height - controls.height

            clip: true

            model: fileModel

            VerticalScrollDecorator { }

            ViewPlaceholder
            {
                text: qsTr('Directory is empty')
                enabled: fileModel.count === 0
            }

            delegate: ListItem
            {
                contentHeight: Theme.itemSizeMedium
                highlighted: down || menuOpen || model.isSelected

                Image
                {
                    id: icon

                    anchors.verticalCenter: parent.verticalCenter

                    x: Theme.horizontalPageMargin

                    source: 'image://theme/icon-m-' + (model.isDir? "folder": "file-other") +
                                (highlighted? '?' + Theme.highlightColor: '')
                }

                Label
                {
                    text: model.fileName

                    anchors.verticalCenter: parent.verticalCenter

                    truncationMode: TruncationMode.Fade

                    anchors.left: icon.right
                    anchors.right: parent.right
                    anchors.leftMargin: Theme.paddingMedium

                    color: highlighted? Theme.highlightColor: Theme.primaryColor
                }

                menu: ContextMenu
                {
                    MenuItem
                    {
                        text: enabled? qsTr('Rename'): qsTr('Renaming is not allowed')

                        enabled: fileSystemHelper.isWritable(model.absolutePath)

                        onClicked: renameFile()
                    }

                    MenuItem
                    {
                        text: enabled? qsTr('Delete'): qsTr('Removal is not allowed')

                        enabled: fileSystemHelper.isRemovable(model.absolutePath)

                        onClicked: deleteFile()
                    }
                }

                onClicked:
                {
                    if (model.isDir)
                        fileModel.path = model.absolutePath;
                    else
                    {
                        if (!model.isSelected)
                            fileModel.clearSelectedFiles();

                        fileModel.toggleSelectedFile(model.index);
                    }
                }

                function deleteFile()
                {
                    remorseAction("Deleting", function() {
                        console.log('Removing ' + model.absolutePath);

                        fileSystemHelper.remove(model.absolutePath);
                    })
                }

                function renameFile()
                {
                    var dlg = pageStack.push('FileNameDialog.qml', {
                        fileName: model.fileName
                    });

                    dlg.accepted.connect(function() {
                        console.log('Renaming ' + model.absolutePath +' to ' + dlg.fileName);

                        fileSystemHelper.rename(model.absolutePath, dlg.fileName);
                    });
                }
            }
        }

        DockedPanel
        {
            id: controls

            width: parent.width
            height: dockedContents.height

            Column
            {
                id: dockedContents

                x: Theme.horizontalPageMargin
                width: parent.width - 2 * x

                spacing: Theme.paddingSmall

                Item
                {
                    width: parent.width
                    height: Theme.paddingMedium
                }

                Label
                {
                    text: fileModel.absolutePath

                    width: parent.width

                    color: Theme.highlightColor

                    truncationMode: TruncationMode.Fade
                    horizontalAlignment: _fadeText? Text.AlignRight: Text.AlignLeft

                    font.pixelSize: Theme.fontSizeExtraSmall
                }

                Row
                {
                    width: parent.width

                    spacing: (width - upIcon.width * 4) / 3

                    IconButton
                    {
                        id: upIcon

                        icon.source: 'qrc:/images/icon-m-up.png'

                        enabled: fileModel.absolutePath !== '/'

                        onClicked: fileModel.path = fileModel.parentPath()
                    }

                    IconButton
                    {
                        icon.source: 'qrc:/images/icon-m-home.png'

                        onClicked: fileModel.path = '/home/nemo'
                    }

                    IconButton
                    {
                        icon.source: 'qrc:/images/icon-m-sdcard.png'

                        enabled: fileSystemHelper.sdCardExists()

                        onClicked: fileModel.path = fileSystemHelper.sdCardPath()
                    }

                    IconButton
                    {
                        icon.source: 'qrc:/images/icon-m-add.png'

                        enabled: fileSystemHelper.isWritable(fileModel.absolutePath)

                        onClicked:
                        {
                            var dlg = pageStack.push('FileNameDialog.qml');

                            dlg.accepted.connect(function() {
                                var dirPath =
                                        fileModel.absolutePath + '/' +  dlg.fileName;

                                console.log('Making path ' + dirPath);

                                if (fileSystemHelper.mkpath(dirPath))
                                    fileModel.path = dirPath;
                            });
                        }
                    }
                }
            }
        }
    }
}
