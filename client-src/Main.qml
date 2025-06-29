import QtQuick
import QtQuick.Shapes
import QtQuick.Controls.Material

Window {
    width: 632
    height: 600
    visible: true
    title: qsTr("Hello World")
    color: "#2e2e2e"
    ServerSetup {}
    Component.onCompleted: {
        console.log(server.get_port())
    }
}
