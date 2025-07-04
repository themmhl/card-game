import QtQuick
import QtQuick.Shapes
import QtQuick.Controls.Material

Item {
    id: root
    anchors.fill: parent
    visible: true
    property var onAccept: function () {}
    Text {
        id: title
        text: "Poffer Card"
        font.family: "Sans Serif"
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
        y: 100
        font.pixelSize: 48
        color: "#0996b5"
    }
    Text {
        text: "Join a server using\nIP Address:"
        font.family: "Noto Sans"
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.top: title.bottom
        anchors.topMargin: 70
        font.pixelSize: 50
        color: "#cccccc"
    }
    MTextBox {
        id: textbox
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: TextInput.AlignLeft
        y: 380
        height: 70
        width: 385
        bgcolor: "#1975e8ff"
        leftPadding: height / 2
        text: "127.0.0.1"
    }
    Text {
        id: status
        text: "Enter the ip address that is shown in the serverside application."
        anchors.top: textbox.bottom
        anchors.topMargin: 4
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#cccccc"
        font.pointSize: 10
    }
    MButton {
        x: 402
        y: 386
        height: 58
        width: 100
        text: "Join"
        onClicked: function () {
            server.set_port(8080)
            server.set_ip_address(textbox.text)
            if (server.connect_to_server()) {
                status.color = "green"
                status.text = "Connected"
                root.onAccept()
            } else {
                status.color = "red"
                status.text = "Error occurred" + ": " + server.get_error()
            }
        }
    }
}
