/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2015 Dmitriy Purgin <dpurgin@gmail.com>

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
    SilicaFlickable {
        anchors.fill: parent

        PageHeader {
            id: header
            title: qsTr('Translators')
        }

        SilicaListView {
            anchors {
                top: header.bottom
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            clip: true

            VerticalScrollDecorator {}

            header: Label {
                id: disclaimer

                text: qsTr('Thanks to these people the Call Recorder is available in different ' +
                           'languages. Names and locale codes are listed in alphabetical order.')

                font.pixelSize: Theme.fontSizeSmall

                width: parent.width

                horizontalAlignment: Text.Center
                wrapMode: Text.Wrap
            }                        

            model: ListModel {
                ListElement {
                    locale: "cs"
                    translators: [
                        ListElement { name: "Jozef Mlích" }
                    ]
                }

                ListElement {
                    locale: "da"
                    translators: [
                        ListElement { name: "Peter Jespersen" }
                    ]
                }

                ListElement {
                    locale: "de"
                    translators: [
                        ListElement { name: "blubdbibub" },
                        ListElement { name: "peter_berlin" },
                        ListElement { name: "Timo Könnecke" },
                        ListElement { name: "Wasilis Mandratzis-Walz" }
                    ]
                }

                ListElement {
                    locale: "el"
                    translators: [
                        ListElement { name: "Wasilis Mandratzis-Walz" }
                    ]
                }

                ListElement {
                    locale: "en"
                    translators: [
                        ListElement { name: "Dmitriy Purgin" }
                    ]
                }

                ListElement {
                    locale: "es"
                    translators: [
                        ListElement { name: "Carmen Fernández B." }
                    ]
                }

                ListElement {
                    locale: "fi"
                    translators: [
                        ListElement { name: "Jukka Aaltonen" },
                        ListElement { name: "Simo Ruoho" }
                    ]
                }

                ListElement {
                    locale: "fr"
                    translators: [
                        ListElement { name: "Jordi" }
                    ]
                }

                ListElement {
                    locale: "it"
                    translators: [
                        ListElement { name: "lorenzo facca" },
                        ListElement { name: "Yuri Bongiorno" }
                    ]
                }

                ListElement {
                    locale: "nl"
                    translators: [
                        ListElement { name: "Heimen Stoffels" },
                        ListElement { name: "HtheB" }
                    ]
                }

                ListElement {
                    locale: "pl"
                    translators: [
                        ListElement { name: "szopin" }
                    ]
                }

                ListElement {
                    locale: "ru"
                    translators: [
                        ListElement { name: "Dmitriy Purgin" }
                    ]
                }

                ListElement {
                    locale: "sk"
                    translators: [
                        ListElement { name: "pyler" }
                    ]
                }

                ListElement {
                    locale: "sv"
                    translators: [
                        ListElement { name: "Åke Engelbrektson" }
                    ]
                }

                ListElement {
                    locale: "zh_TW"
                    translators: [
                        ListElement { name: "hanhsuan" }
                    ]
                }
            }

            delegate: Column {
                width: parent.width - 2 * Theme.paddingLarge
                height: spacer.height + contentRow.height
                x: Theme.paddingLarge

                Item {
                    id: spacer

                    width: parent.width
                    height: Theme.paddingMedium
                }

                Row {
                    id: contentRow

                    width: parent.width

                    Label {
                        text: locale

                        width: parent.width * 0.25

                        font.pixelSize: Theme.fontSizeExtraSmall

                        color: Theme.secondaryColor
                    }

                    Column {
                        width: parent.width * 0.75

                        Repeater {
                            model: translators

                            Label {
                                text: name

                                font.pixelSize: Theme.fontSizeExtraSmall
                            }
                        }
                    }
                }
            }
        }
    }
}
