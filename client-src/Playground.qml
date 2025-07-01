import QtQuick

Item {
    id: root
    anchors.fill: parent
    property string username: "Lil mane mane"
    property string opp_username: "Nigga creep"
    Connections {
        target: server

        function onAdd_card(suit, rank, where) {
            //make sure to clear them first
            //where : MY_SELECTED, MY_AVAILABLE, OPP_SELECTED, OPP_AVAILABLE
            switch (where) {
            case 0:
                my_selected.add_card(suit, rank)
                break
            case 1:
                my_available.add_card(suit, rank)
                break
            case 2:
                opp_selected.add_card(suit, rank)
                break
            case 3:
                opp_available.add_card(suit, rank)
                break
            }
        }

        function onClear() {
            opp_available.clearCards()
            opp_selected.clearCards()
            my_selected.clearCards()
            my_available.clearCards()
        }

        function onShow_status(text) {
            status.visible = true
            status.status_text = text
            status.opacity = 1
            pause.running = true
        }
    }

    PauseAnimation {
        id: pause
        duration: 4000
        onFinished: {
            status.visible = false
            status.opacity = 0
        }
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
        height: 74
        width: parent.width
        color: "#99800069"
        radius: 32
        MButton {
            text: "Open Chat"
            x: 10
            y: 10
            height: 54
            width: 180
            onClicked: function () {
                status.visible = true
                status.status_text = "test shit"
                status.opacity = 1
                pause.running = true
            }
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20
            y: 10
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
            y: 10
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
        id: my_available
        anchors.bottom: root.bottom
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        anchors.left: root.left
        height: 300
        width: 600
        select_enabled: true
    }

    CardPanel {
        id: my_selected
        anchors.bottom: root.bottom
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        anchors.right: root.right
        height: 300
        width: 600
        rtl: true
    }
    Column {
        spacing: 10
        anchors.centerIn: parent

        Row {
            spacing: 8
            Rectangle {
                height: 60
                width: 60
                radius: 30
                color: "#e63600"
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
                color: "black"
            }
        }
        Row {
            spacing: 8
            Rectangle {
                height: 60
                width: 60
                radius: 30
                color: "#00b7d3"
                Text {
                    text: root.opp_username.charAt(0)
                    font.pointSize: 24
                    anchors.centerIn: parent
                    color: "white"
                }
            }
            Text {
                text: root.opp_username
                anchors.verticalCenter: parent.verticalCenter
                font.family: "Noto Sans"
                font.pixelSize: 32
                color: "black"
            }
        }
    }
    Rectangle {
        id: status
        anchors.fill: parent
        color: "#b3a3a3a3"
        visible: false
        property string status_text: "You Lost."
        opacity: 0
        Behavior on opacity {
            NumberAnimation {
                duration: 500
            }
        }
        Text {
            id: titleText
            text: parent.status_text
            anchors.centerIn: parent
            font.pointSize: 80
            font.bold: true
            color: "black"
        }
    }
}
