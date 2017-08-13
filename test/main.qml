import QtQuick 2.7
import QtQuick.Controls 2.1

ApplicationWindow {
    visible: true
    width: 700
    height: 600
    property var dm: DeviceManager
    property var deviceList: DeviceManager.deviceList

    onDeviceListChanged: {
        console.log(deviceList.length)
    }

    ListView {
        width: parent.width
        height: parent.height
        model: deviceList
        spacing: 20

        delegate:  Rectangle {
            border.width: 1
            border.color: "red"
            height: 40
            width: parent.width - 100
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 20
            color: "transparent"

            Text {
                anchors.centerIn: parent
                text: modelData.name
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log(typeof modelData)
                    var service = dm.connectToDevice(modelData)
                    console.log(service)
                    console.log(service.type())
                    console.log(service.execute("get weight"))
                    /*
                    service.onServiceConnected.connect( function() {
                        console.log("666")
                        console.log(service.execute("get weight"))
                    })
                    */
                }
            }
        }
    }

    /*
    Connections {
        target: dm
        onFinished: {
            console.log("finished")
        }
    }
    */

    /*
    dm.onUpdated: {
        //console.log("updated")
        //console.log(dm.deviceList.length())
    }
*/

    Component.onCompleted: {
        dm.scan()
    }
}
