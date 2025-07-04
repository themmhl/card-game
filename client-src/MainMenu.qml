import QtQuick
import QtQuick.Shapes
import QtQuick.Controls.Material
import QtQuick.Dialogs

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
    property var onedit:function () {
    }
    Connections {
        target: server

        function onStatus_changed(data, color) {
            status.text = data
            status.color = color
            if (data === "Opponent found. Starting match...") {
                status.text = "Join A Match"
                root.ongamestart()
            }
        }
    }
    Item {
        id: main_menu_root
        anchors.fill: parent
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
            text: "Join A Match"
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
                } else {
                    status.color = "lightblue"
                    //status.text = "Waiting for other players to join..." + server.get_server_message(6)
                    status.text = "Waiting for other\nplayers to join..."
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
                main_menu_root.visible = false
                history.visible = true
                server.get_history()
                history.history_string = server.get_history_string()
            }
        }
        MButton {
            x: 331
            y: 624
            width: 180
            height: 60
            text: "Edit Info"
            onClicked: function () {
                root.onedit()
                main_menu_root.visible = false;
                edit_info_root.visible = true;

                name.text = server.getPlayerName();
                surname.text = server.getPlayerSurname();
                email.text = server.getPlayerEmail();
                phone_number.text = server.getPlayerPhoneNumber();
                usernamee.text = server.getPlayerUsername();

                passwordd.text = "";
            }
        }
        MButton {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: pfp.bottom
            anchors.topMargin: 100
            width: 180
            height: 60
            text: "Logout"
            onClicked: function () {
                logoutDialog.open() //open the dialog
            }
        }
    }
    MessageDialog {
        id: logoutDialog
        title: "Confirm Logout"
        text: "Are you sure you want to logout?"
        buttons: MessageDialog.Yes | MessageDialog.No
        onAccepted: {
            if (server.logout()) {
                root.onlogout()
            } else {
                status.text = "Logout failed. Error: "+ server.get_server_message(2);
                status.color = "red";
            }
        }
    }
    History {
        id: history
        anchors.fill: parent
        visible: false
        onreturn: function () {
            history.visible = false
            main_menu_root.visible = true
            edit_info_root.visible = false
        }
    }
    Item {
        id: edit_info_root
        visible: false
        anchors.fill: parent
        Text {
            id: titleeee_edit
            text: qsTr("Edit Info")
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 24
            font.bold: true
            color: "white"
        }

        MTextBox {
            id: name_edit
            width: parent.width * 0.6
            height: 64
            anchors.top: titleeee_edit.bottom
            anchors.topMargin: 60
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
        }
        Text {
            text: "Name:"
            anchors.bottom: name_edit.top
            anchors.bottomMargin: 4
            anchors.left: name_edit.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: surname_edit
            width: parent.width * 0.6
            height: 64
            anchors.top: name_edit.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
        }
        Text {
            text: "Last Name:"
            anchors.bottom: surname_edit.top
            anchors.bottomMargin: 4
            anchors.left: surname_edit.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: phone_number_edit
            width: parent.width * 0.6
            height: 64
            anchors.top: surname_edit.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
        }
        Text {
            text: "Phone Number:"
            anchors.bottom: phone_number_edit.top
            anchors.bottomMargin: 4
            anchors.left: phone_number_edit.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: email_edit
            width: parent.width * 0.6
            height: 64
            anchors.top: phone_number_edit.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
        }
        Text {
            text: "Email address:"
            anchors.bottom: email_edit.top
            anchors.bottomMargin: 4
            anchors.left: email_edit.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: username_edit
            width: parent.width * 0.6
            height: 64
            anchors.top: email_edit.bottom
            anchors.topMargin: 60
            anchors.horizontalCenter: parent.horizontalCenter
            //r text: ""
        }
        Text {
            text: "Username"
            anchors.bottom: username_edit.top
            anchors.bottomMargin: 4
            anchors.left: username_edit.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: password_edit
            width: parent.width * 0.6
            height: 64
            anchors.top: username_edit.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            // text: ""
            echoMode: TextInput.Password
        }
        Text {
            text: "Password"
            anchors.bottom: password_edit.top
            anchors.bottomMargin: 4
            anchors.left: password_edit.left
            color: "#cccccc"
            font.pointSize: 10
        }
        Text {
            id: status_edit
            text: ""
            anchors.top: password_edit.bottom
            visible: false
            anchors.topMargin: 4
            anchors.horizontalCenter: parent.horizontalCenter
            color: "red"
            font.pointSize: 10
        }
        MButton {
            id:savebtn
            text: "Save Changes"
            width: parent.width * 0.6
            height: 64
            anchors.top: password_edit.bottom
            anchors.topMargin: 60
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: function () {
                status_edit.visible = true
                if (server.edit_info(name_edit.text, surname_edit.text, email_edit.text,
                    phone_number_edit.text, username_edit.text,
                    password_edit.text)) {
                        root.username = username_edit.text;
                    status_edit.text = "Information updated successfully.";
                    status_edit.color = "green";
                } else {
                    status_edit.text = "Error updating information: " + server.get_server_message(3);
                    status_edit.color = "red";
                }
            }
        }
        MButton {
            text: "Return"
            width: parent.width * 0.6
            height: 64
            anchors.top: savebtn.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: function () {
                edit_info_root.visible = false;
                main_menu_root.visible = true;
            }
        }
    }
}
