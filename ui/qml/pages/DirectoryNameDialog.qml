import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    property alias directoryName: textField.text

    SilicaFlickable {
        anchors.fill: parent

        DialogHeader {
            id: header
        }

        TextField {
            id: textField

            anchors.top: header.bottom

            width: parent.width            

            placeholderText: qsTr('Directory name')
            label: qsTr('Directory name')

            focus: true

            validator: RegExpValidator {
                regExp: /[^\/]+/
            }
        }
    }

    Component.onCompleted: {
        //textField.forceActiveFocus();
    }
}
