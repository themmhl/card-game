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
    property var ongamestart: function () {
    }
    property var onlogout: function () {
    }
    Connections {
        target: server

        function onStatus_changed(data, color) {
            status.text = data
            status.color = color
            if (data === "Opponent found. Starting match...") {
                root.ongamestart();
            }
        }
    }

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
        id: status
        text: "Status"
        font.family: "Noto Sans"
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.top: title.bottom
        anchors.topMargin: 70
        font.pixelSize: 50
        color: "#cccccc"
    }
    MButton {
        width: 200
        height: 60
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: status.bottom
        anchors.topMargin: 70
        text: "Join a Match"
        onClicked: function () {
            if (!server.join_game()) {
                status.color = "red"
                status.text = "Error: " + server.get_server_message(6)
            }
            else{
                status.color = "lightblue"
                status.text = "Waiting for other players to join..."
            }
        }
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
        onClicked: function () {
            //history is unfinished
        }
    }
    MButton {
        x: 331
        y: 624
        width: 180
        height: 60
        text: "Logout"
        onClicked: function () {
            if (server.logout()) {
                onlogout()
            }
        }
    }
}
