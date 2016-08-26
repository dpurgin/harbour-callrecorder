import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: licensePage

    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors {
            fill: parent

            leftMargin: Theme.paddingLarge
            rightMargin: Theme.paddingLarge
        }

        contentHeight: contentColumn.height

        Column {
            id: contentColumn


            width: parent.width

            PageHeader {
                //% "License"
                title: qsTrId('id_license')
            }

            Label {
                anchors {
                    leftMargin: Theme.paddingLarge
                    rightMargin: Theme.paddingLarge
                }

                width: parent.width

                font.pixelSize: Theme.fontSizeTiny

                wrapMode: Text.Wrap

                text: license
            }
        }

        VerticalScrollDecorator {}
    }
}
