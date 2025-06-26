import QtQuick
import QtQuick.Shapes
import QtQuick.Controls.Material

Window {
    width: 500
    height: 720
    visible: true
    title: qsTr("Hello World")
    color: "black"
    Text{
        id:ip_address
        text: "127.0.0.1"
        x: 76
        y: 114
        font.family: "SegoeUI, Segoe UI"
        font.underline: true
        color: "#75e8ff"
    }
}
