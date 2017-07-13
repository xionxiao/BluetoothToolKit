import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 680
    height: 420
    title: qsTr("Bluetooth")

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Page {
            Label {
                text: qsTr("Bluetooth Updater")
                anchors.centerIn: parent
                color: "#2196F3" // Material.blue
            }
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("Device")
        }
        TabButton {
            text: qsTr("About")
        }
    }
}
