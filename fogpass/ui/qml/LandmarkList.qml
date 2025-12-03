import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    width: 300
    height: 200
    model: ["Bridge", "Tunnel", "Station"]
    delegate: Text {
        text: modelData
        font.pixelSize: 18
    }
}
