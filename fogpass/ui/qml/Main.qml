import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: root
    visible: true
    width: 800
    height: 480
    title: "Fogpass"

    Column {
        anchors.centerIn: parent
        spacing: 12
        Text { text: "Fogpass Dashboard"; font.pixelSize: 28 }
        Loader { source: "Speed.qml" }
        Loader { source: "LandmarkList.qml" }
    }
}
