import QtQuick

Window {
    id: root
    width: 632
    height: login_page.heightt
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
        onVisibleChanged: {
            if (visible) {
                root.height = heightt
            }
        }
        onloginsuccess: function () {
            login_page.visible = false
            main_menu.visible = true
            main_menu.username = login_page.username
        }
        onsignup: function () {
            root.height = 1000
        }

        onreturn: function () {
            root.height = 600
        }
    }
    MainMenu {
        id: main_menu
        visible: false
        onVisibleChanged: {
            if (visible) {
                root.height = 800
            }
        }
        ongamestart: function () {
            visible = false
            playground.visible = true
        }

        onlogout: function () {
            main_menu.visible = false
            root.height= 600
            login_page.visible = true
        }
        onedit: function () {
            root.height = 1000
        }
    }
    Playground {
        id: playground
        visible: false
        anchors.fill: parent
        onVisibleChanged: {
            if (visible) {
                root.height = 900
                root.width = 1280
            }
        }
        onReturn:function(){
            playground.visible = false
            main_menu.visible = true
            root.height = 1000
            root.width= 632
        }
    }
}
