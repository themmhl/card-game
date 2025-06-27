import QtQuick
import QtQuick.Shapes
import QtQuick.Controls.Material

Item {
    id: root
    anchors.fill: parent
    visible: true
    width: 632
    height: 800
    property string username: "Mohammad mahdi"
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
        text: "Join a match using\nIP Address:"
        font.family: "Noto Sans"
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.top: title.bottom
        anchors.topMargin: 70
        font.pixelSize: 50
        color: "#cccccc"
    }
    Text {
        text: "Logged in as:"
        anchors.bottom: pfp.top
        anchors.bottomMargin: 4
        anchors.left: pfp.left
        color: "#cccccc"
        font.pointSize: 10
    }
    Row {
        id: pfp
        y: 534
        spacing: 8
        anchors.horizontalCenter: parent.horizontalCenter
        Rectangle {
            height: 60
            width: 60
            radius: 30
            color: "purple"
            Text {
                text: root.username.charAt(0)
                font.pointSize: 24
                anchors.centerIn: parent
                color: "white"
            }
        }
        Text {
            text: root.username
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Noto Sans"

            font.pixelSize: 32
            color: "#cccccc"
        }
    }
    MButton {
        x: 126
        y: 624
        width: 180
        height: 60
        text: "History"
    }
    MButton {
        x: 331
        y: 624
        width: 180
        height: 60
        text: "Logout"
    }
    MTextBox {
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: TextInput.AlignLeft
        y: 380
        height: 70
        width: 385
        bgcolor: "#1975e8ff"
    }
    MButton {
        x: 402
        y: 386
        height: 58
        width: 100

        text: "Join"
    }
}
