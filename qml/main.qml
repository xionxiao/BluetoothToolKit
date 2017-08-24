import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import QtQuick.Dialogs 1.2
import "./workerscript.js" as WScript

ApplicationWindow {
    id: app_window
    visible: true
    width: 680
    height: 420
    title: qsTr("BluetoothToolbox")
    property var dm: DeviceManager

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
            text: qsTr("Bluetooth: " + DeviceManager.powerState)
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
            onClicked: {
                fs.open()
            }
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y: 270
            text: DeviceManager.address
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y: 300
            text: DeviceManager.name
        }
    }

    Frame {
        id: frame
        x: 260
        width: parent.width - 260 - 20
        height: parent.height - 100
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.rightMargin: 20
        padding: 0

        Item {
            id: header
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
                id: busy_indicator
                anchors.right: parent.right
                width: 24
                height:24
                running: false
            }

            Rectangle {
                anchors.bottom: parent.bottom
                height: 1
                width: parent.width
                color: Material.frameColor
            }
        }

        ListView {
            id: deviceListView
            width: parent.width
            height: parent.height - 24
            clip: true
            anchors.top: header.bottom
            currentIndex: -1

            delegate: Rectangle {
                id: delegate_item
                width: deviceListView.width
                height: item_services.height + 48 + 10
                color: ListView.isCurrentItem ? Material.color(Material.Blue) : "transparent"
                property alias item_services: item_services
                property bool highlighted: ListView.isCurrentItem
                Behavior on height {
                    NumberAnimation { duration: 200 }
                }
                Image {
                    width: 48
                    height: 48
                    anchors.verticalCenter: parent.verticalAlignment
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    source: "Bluetooth.png"
                    fillMode: Image.PreserveAspectFit
                }
                Item {
                    id: item_name
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.leftMargin: 48 + 5
                    width: parent.width
                    height: 24
                    Text {
                        anchors.bottom: parent.bottom
                        text: modelData.name ? modelData.name : "Unnamed"
                        font.bold: highlighted
                        color: highlighted ? "white" : "black"
                    }
                }
                Item {
                    id: item_rssi
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.rightMargin: 48 + 5
                    height: 24
                    Text {
                        anchors.bottom: parent.bottom
                        text: modelData.rssi
                        color: "black"
                    }
                }
                Item {
                    id: item_uuid
                    anchors.top: item_name.bottom
                    anchors.topMargin: 5
                    anchors.left: parent.left
                    anchors.leftMargin: 48 + 5
                    height: 24
                    Text {
                        anchors.top: parent.top
                        text: modelData.uuid.toUpperCase()
                        color: highlighted ? "white" : "gray"
                    }
                }
                MouseArea {
                    ListModel {id:listModel}
                    anchors.fill: parent
                    onClicked: {
                        deviceListView.currentIndex = highlighted ? -1 : index
                        console.log(modelData)
                        listModel.append({"dm":DeviceManager, "device":modelData})
                        var msg = {'params': listModel}
                        console.log(msg)
                        worker.sendMessage(msg)
                    }
                }
                ListView {
                    id: item_services
                    anchors.top: item_uuid.bottom
                    width: parent.width
                    height: highlighted ? contentHeight : 0
                    visible: highlighted
                    spacing: 5
                    delegate: Rectangle {
                        color: "transparent"
                        width: parent.width
                        height: 24
                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 24
                            anchors.verticalCenter: parent.verticalCenter
                            text: modelData.name
                            color: "white"
                        }
                        Rectangle {
                            anchors.top: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: parent.width - 48
                            height: 1
                            color: "white"
                        }
                    }

                }
                Button {
                    id: download_button
                    /* temporary name filter with boot */
                    visible: parent.highlighted && modelData.name == "boot"
                    anchors.right: parent.right
                    height: 48
                    width: 48
                    contentItem: Image {
                        anchors.centerIn: parent
                        width: 18
                        height: 18
                        source: "download.png"
                        fillMode: Image.PreserveAspectFit
                    }
                    background: Rectangle {
                        anchors.centerIn: parent
                        color: download_button.down ? Material.color(Material.Green) : "transparent"
                        width: 36
                        height: 36
                        radius: 24
                    }
                }
            }
            model: DeviceManager.deviceList
        }
    }

    Button {
        id: refresh_button
        anchors.top: frame.bottom
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 20
        width: 120
        height: 36
        text: qsTr("Refresh")
        font.capitalization: Font.MixedCase
        highlighted: true
        enabled: false
        onClicked: {
            busy_indicator.running = true
            DeviceManager.scan()
        }
    }

    CheckBox {
        id: checkbox
        anchors.left: frame.left
        anchors.leftMargin: -8
        anchors.top: frame.bottom
        anchors.topMargin: 10
        height: 36
        text: qsTr("display all devices")
        checked: true
    }

    FileDialog {
        id: fs
        title: "Please choose the firmware"
        folder: shortcuts.home
        onAccepted: {
            console.log("You chose: " + fileUrl)
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    WorkerScript {
        id: worker
        source: "workerscript.js"
        onMessage: {
            console.log(messageObject.reply)
        }

    }

    Component.onCompleted: {
        refresh_button.enabled = true
        if (DeviceManager.isValid()) {
            busy_indicator.running = true;
            DeviceManager.scan()
        }
    }

    Connections {
        target: DeviceManager
        onUpdated: {
            busy_indicator.running = false
            deviceListView.currentIndex = -1
        }
        onError: {
            console.log(busy_indicator.running)
            busy_indicator.running = false
            console.log(errorCode, errorString)
        }
        onDeviceConnected: {
            console.log("connected", services)
            deviceListView.currentItem.item_services.model = services
        }
    }
}
