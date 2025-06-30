import QtQuick

Item {
    id: root
    anchors.fill: parent
    property string username: "lil mane mane"
    Component.onCompleted: {

    }
    Image {
        id: img
        source: "qrc:/card-pics/Game-Ground-Photo.JPG"
        anchors.fill: parent
        z: -1
    }
    Rectangle {
        y: 8
        id: menu
        height: 64
        width: parent.width
        color: "#99800069"
        radius: 32
        MButton {
            text: "Open Chat"
            x: 10
            y: 5
            height: 54
            width: 180
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20
            y: 5
            MButton {
                text: "Resume"
                height: 54
                width: 180
            }
            MButton {
                text: "Flip"
                height: 54
                width: 180
            }
        }
        MButton {
            text: "Exit"
            anchors.right: parent.right
            y: 5
            anchors.rightMargin: 10
            height: 54
            width: 180
        }
    }
    CardPanel {
        id: opp_available
        anchors.top: menu.bottom
        anchors.topMargin: 10
        anchors.leftMargin: 10
        anchors.left: root.left
        height: 393
        width: 600
    }

    CardPanel {
        id: opp_selected
        anchors.top: menu.bottom
        anchors.topMargin: 10
        anchors.rightMargin: 10
        anchors.right: root.right
        height: 393
        width: 600
        rtl: true
    }

    CardPanel {
        id: user_available
        anchors.bottom: root.bottom
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        anchors.left: root.left
        height: 300
        width: 600
    }

    CardPanel {
        id: user_selected
        anchors.bottom: root.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        anchors.right: root.right
        height: 300
        width: 600
        rtl: true
    }
}
