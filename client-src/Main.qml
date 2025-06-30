import QtQuick

Window {
    id: root
    width: 632
    height: 600
    visible: true
    title: qsTr("Hello World")
    color: "#2e2e2e"

    ServerSetup {
        id: server_page
        onAccept: function () {
            server_page.visible = false
            login_page.visible = true
            root.height = login_page.heightt
        }
    }
    Login {
        id: login_page
        visible: false
        Component.onCompleted: {
            root.height = heightt
        }
        onloginsuccess: function () {
            login_page.visible = false
            main_menu.visible = true
            main_menu.username = login_page.username
        }
    }
    MainMenu {
        id: main_menu
        visible: false
        ongamestart: function () {
            visible = false
            playground.visible = true
        }

        onlogout:function(){
            main_menu.visible = false
            login_page.visible = true
        }
    }
    Playground {
        id: playground
    }
    Card {
        width: 305
        height: 400
        suit: 3
        rank: 3
        anchors.centerIn: parent
    }
    Playground {
        anchors.fill: parent
        Component.onCompleted: {
            root.height = 720
            root.width = 1280
        }
    }
}
