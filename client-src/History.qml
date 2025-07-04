import QtQuick

Item {
    id: history_root
    anchors.fill: parent
    property var onreturn: function () {}
    property string history_string: ""
    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        y: 50
        spacing: 10

        Text {
            text: "History"
            font.family: "Sans Serif"
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.pixelSize: 36
            color: "darkgrey"
        }
        // Opponent Username    Date           Rounds   Result
        Text {
            text: "Opponent Username    Date           Rounds   Result"
            font.family: "Sans Serif"
            font.pixelSize: 24
            color: "white"
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Rectangle {
            height: 3
            width: history_text.width + 20
            color: "gray"
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Text {
            id: history_text
            text: history_root.history_string
            font.family: "Courier New"
            font.pixelSize: 18
            color: "lightgrey"
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignLeft
        }
        Rectangle {
            height: 50
            color: "transparent"
            width: 1
        }
        MButton {
            height: 58
            width: 150
            text: "Return to Menu"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: function () {
                history_root.onreturn()
            }
        }
    }
}
