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

Page {
    id: aboutPage

    property int spacerHeight: isPortrait? Theme.paddingLarge * 2: Theme.paddingMedium

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent

        contentHeight: contentColumn.height

        PullDownMenu {
            MenuItem {
                //: Pull-down menu entry
                //% "Translators"
                text: qsTrId('id_translators')
                onClicked: {
                    pageStack.push(Qt.resolvedUrl('Translators.qml'))
                }
            }

            MenuItem {
                //: Pull-down menu entry
                //% "License"
                text: qsTrId('id_license')
                onClicked: {
                    pageStack.push(Qt.resolvedUrl('License.qml'))
                }
            }
        }

        Column {
            id: contentColumn

            width: parent.width

            anchors {
                fill: parent

                leftMargin: Theme.paddingLarge
                rightMargin: Theme.paddingLarge
            }

            PageHeader {
                id: pageHeader

                //: Page header
                //% "About"
                title: qsTrId('id_settings_about')
            }

            Column {
                id: headerColumn

                width: parent.width

                Item {
                    width: parent.width
                    height: spacerHeight
                }

                Label {
                    horizontalAlignment: Text.AlignHCenter

                    width: parent.width

                    font.pixelSize: Theme.fontSizeExtraLarge
                    color: Theme.primaryColor

                    //: Application title
                    //% "Call Recorder"
                    text: qsTrId('id_call_recorder')

                    wrapMode: Text.Wrap
                }

                Label {
                    horizontalAlignment: Text.AlignHCenter

                    width: parent.width

                    font.pixelSize: Theme.fontSizeLarge
                    color: Theme.secondaryColor

                    //: App subtitle on About page
                    //% "for SailfishOS"
                    text: qsTrId('id_for_sailfishos')

                    wrapMode: Text.Wrap
                }

                Label {
                    horizontalAlignment: Text.AlignHCenter

                    width: parent.width

                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.primaryColor

                    //: Version on About page
                    //% "Version %1"
                    text: qsTrId('id_version').arg(VERSION)
                }

                Item {
                    width: parent.width
                    height: spacerHeight
                }

                Label {
                    //: Copyright string
                    //% "Copyright \u00a9 2014-2016 Dmitriy Purgin"
                    text: qsTrId('id_about_copyright')

                    horizontalAlignment: Text.AlignHCenter

                    width: parent.width

                    font.pixelSize: Theme.fontSizeExtraSmall
                }

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter

                    font.pixelSize: Theme.fontSizeTiny
                    color: Theme.secondaryColor

                    text: 'dpurgin@gmail.com'
                }

                Item {
                    width: parent.width
                    height: Theme.paddingMedium
                }

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter

                    font.pixelSize: Theme.fontSizeTiny
                    color: Theme.highlightColor

                    text: "https://github.com/dpurgin/harbour-callrecorder"
                }

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter

                    font.pixelSize: Theme.fontSizeTiny
                    color: Theme.highlightColor

                    text: "https://www.transifex.com/projects/p/harbour-callrecorder/"
                }

                Item {
                    width: parent.width
                    height: spacerHeight
                }

                Label {
                    //: Acknowledgment text on About page
                    //% "Thanks to Simonas Leleiva and Juho Hämäläinen"
                    text: qsTrId('id_about_thanks')

                    font.pixelSize: Theme.fontSizeExtraSmall
                    width: parent.width

                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                }

                Label {
                    //: Clarification text on About page
                    //% "Use pull-down menu to see translators"
                    text: qsTrId('id_use_pulldown_translators')

                    font.pixelSize: Theme.fontSizeTiny
                    width: parent.width

                    color: Theme.secondaryColor

                    wrapMode: Text.Wrap
                    horizontalAlignment: Text.AlignHCenter
                }
            }                        

            Item {
                width: parent.width
                height: aboutPage.height - pageHeader.height - headerColumn.height -
                        licenseColumn.height - Theme.paddingLarge
            }

            Column {
                id: licenseColumn

                width: parent.width

                Label {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }

                    width: parent.width
                    wrapMode: Text.Wrap

                    font.pixelSize: Theme.fontSizeTiny

                    color: Theme.secondaryColor

                    //: Short GPL disclaimer on About page
                    //% "This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under certain conditions; use pull-down menu for details"
                    text: qsTrId('id_gpl_disclaimer')
                }
            }
        }
    }
}
