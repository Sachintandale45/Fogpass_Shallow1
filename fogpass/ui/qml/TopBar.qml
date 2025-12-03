import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    width: parent ? parent.width : 800
    height: 48
    color: "#101010"

    Row {
        anchors.verticalCenter: parent.verticalCenter
        spacing: 24
        Text { text: "Fogpass"; color: "white"; font.pixelSize: 20 }
        Text { text: "GPS Locked"; color: "lightgreen"; font.pixelSize: 14 }
    }
}
