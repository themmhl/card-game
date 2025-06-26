import QtQuick
import QtQuick.Shapes
import QtQuick.Controls.Material

Window {
    width: 632
    height: 800
    visible: true
    title: qsTr("Hello World")
    color: "black"
    Text {
        id: ip_address
        text: "127.0.0.1"
        font.family: "Source Code Pro"
        x: 107
        y: 218
        font.underline: true
        font.pixelSize: 100
        color: "#75e8ff"
    }
    Text {
        text: "Poffer Card"
        font.bold: true
        x: 199
        y: 92
        font.pixelSize: 44
        color: "#6588ce"
    }
    Text {
        text: "Or join an existing match:"
        font.family: "Noto Sans"
        x: 126
        y: 413
        font.pixelSize: 32
        color: "#bd6687cf"
    }

    Text {
        text: "Enter this in other player' instances:"
        font.family: "Noto Sans"
        x: 49
        y: 162
        font.pixelSize: 32
        color: "#bd6687cf"
    }
    Text {
        text: "logged in as:   fghjkldfg"
        font.family: "Noto Sans"
        x: 134
        y: 670
        font.pixelSize: 32
        color: "#bd6687cf"
    }
    MButton {
        x: 126
        y: 574
        width: 180
        height: 60
        text: "History"
    }
    MButton {
        x: 331
        y: 574
        width: 180
        height: 60
        text: "Logout"
    }
    MTextBox {
        x: 126
        y: 480
        height: 70
        width: 385
        bgcolor: "#1975e8ff"
    }
    MButton {
        x: 446
        y: 486
        height: 58
        width: 58
        initial_color: "green"
        color_opacity: 0.7
        text: "GO"
        onClicked: function () {
            if (initial_color == "green") {
                initial_color = "red"
            } else {
                initial_color = "green"
            }
        }
    }
}
