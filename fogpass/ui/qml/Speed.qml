import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    width: 200
    height: 120
    color: "#202020"
    radius: 8

    Text {
        anchors.centerIn: parent
        text: "42 km/h"
        font.pixelSize: 32
        color: "white"
    }
}
