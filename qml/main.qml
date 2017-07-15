import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2

ApplicationWindow {
    visible: true
    width: 680
    height: 420
    title: qsTr("Bluetooth")

    Item {
        id: left_panel
        width: 260
        height: 420

        Image {
            id: image
            height: 200
            clip: true
            fillMode: Image.PreserveAspectFit
            Layout.fillHeight: false
            Layout.fillWidth: false
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 8
            source: "Bluetooth.jpg"
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y: 210
            text: qsTr("Bluetooth: On")
            font.bold: true
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            y: 230
            width: 120
            height: 36
            text: qsTr("Firmware")
            font.capitalization: Font.MixedCase
            highlighted: true
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y: 270
            text: qsTr("Now discoverable as")
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y: 300
            text: qsTr("\"Allen\"")
        }
    }

    Frame {
        id: device_frame
        x: 260
        width: parent.width - 260 - 20
        height: parent.height - 100
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.rightMargin: 20
        padding: 0

        Item {
            anchors.top: parent.top
            width: parent.width
            height: 24

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10
                text: "Devices"
                color: "gray"
                font.bold: true
            }

            BusyIndicator {
                anchors.right: parent.right
                width: 24
                height:24
            }

            Rectangle {
                anchors.bottom: parent.bottom
                height: 1
                width: parent.width
                color: Material.frameColor
            }
        }
    }

    Button {
        id: button
        anchors.top: device_frame.bottom
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 20
        width: 120
        height: 36
        text: qsTr("Refresh")
        font.capitalization: Font.MixedCase
        highlighted: true
    }

    CheckBox {
        id: checkbox
        anchors.left: device_frame.left
        anchors.leftMargin: -8
        anchors.top: device_frame.bottom
        anchors.topMargin: 10
        height: 36
        text: qsTr("display all devices")
        checked: true
    }
}
