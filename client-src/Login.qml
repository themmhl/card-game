import QtQuick
import QtQuick.Controls

Item {
    id: root
    anchors.fill: parent
    property alias username: usernameInput.text
    property alias password: passwordInput.text
    property int heightt: 600
    property var onloginsuccess: function () {}
    property var onsignup: function (){}
    property var onreturn: function (){}
    Item {
        id: loginroot
        anchors.fill: parent

        Text {
            id: titleText
            text: qsTr("Login")
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 24
            font.bold: true
            color: "white"
        }

        MTextBox {
            id: usernameInput
            width: parent.width * 0.6
            height: 64
            anchors.top: titleText.bottom
            anchors.topMargin: 60
            anchors.horizontalCenter: parent.horizontalCenter
            //r text: ""
        }
        Text {
            text: "Username"
            anchors.bottom: usernameInput.top
            anchors.bottomMargin: 4
            anchors.left: usernameInput.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: passwordInput
            width: parent.width * 0.6
            height: 64
            anchors.top: usernameInput.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            // text: ""
            echoMode: TextInput.Password
        }
        Text {
            text: "Password"
            anchors.bottom: passwordInput.top
            anchors.bottomMargin: 4
            anchors.left: passwordInput.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MButton {
            id: loginButton
            text: "Login"
            width: parent.width * 0.6
            height: 64
            anchors.top: passwordInput.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: function () {
                console.log("Login clicked")
                console.log("Username: " + root.username)
                console.log("Password: " + root.password)
                if (server.login(root.username, root.password)) {
                    loginroot.visible = true
                    root.heightt = 800
                    status.visible = false
                    onloginsuccess()
                } else {
                    status.visible = true
                    status.text = server.get_server_message(
                                0) + ": " + server.get_error()
                }
            }
        }
        Text {
            id: status
            text: ""
            anchors.top: loginButton.bottom
            visible: false
            anchors.topMargin: 4
            anchors.horizontalCenter: parent.horizontalCenter
            color: "red"
            font.pointSize: 10
        }
        Row {
            anchors.top: loginButton.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.6
            height: 40
            spacing: parent.width * 0.02
            MButton {
                text: "Forgot Password?"
                width: parent.parent.width * 0.29
                height: 40
                initial_color: Qt.hsva(0, 0, 0, 0)
                color_opacity: 0.1
                onClicked: function () {
                    loginroot.visible = false
                    forgotroot.visible = true
                    root.heightt = 500
                }
            }
            MButton {
                text: qsTr("Sign up")
                width: parent.parent.width * 0.29
                height: 40
                initial_color: Qt.hsva(0, 0, 0, 0)
                color_opacity: 0.1
                onClicked: function () {
                    onsignup()
                    loginroot.visible = false
                    signuproot.visible = true
                    root.heightt = 1000
                }
            }
        }
    }

    Item {
        id: forgotroot
        visible: false
        anchors.fill: parent
        Text {
            id: titleee
            text: qsTr("Password Recovery")
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 24
            font.bold: true
            color: "white"
        }

        MTextBox {
            id: phonenumber
            width: parent.width * 0.6
            height: 64
            anchors.top: titleee.bottom
            anchors.topMargin: 60
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
        }
        Text {
            text: "Phone number associated with your account:"
            anchors.bottom: phonenumber.top
            anchors.bottomMargin: 4
            anchors.left: phonenumber.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: newpass
            width: parent.width * 0.6
            height: 64
            anchors.top: phonenumber.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
            echoMode: TextInput.Password
        }
        Text {
            text: "New Password"
            anchors.bottom: newpass.top
            anchors.bottomMargin: 4
            anchors.left: newpass.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MButton {
            id: change_password_button
            text: "Change Password"
            width: parent.width * 0.6
            height: 64
            anchors.top: newpass.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: function () {
                console.log("Username: " + phonenumber.text)
                console.log("Password: " + newpass.text)
                if (server.forgot_password(phonenumber.text, newpass.text)) {
                    statuss.visible = true
                    statuss.color = "green"
                    statuss.text = "Password accociated with number "
                            + phonenumber.text + "\nhas been successfully changed."
                } else {
                    statuss.visible = true
                    statuss.text = server.get_server_message(4)
                }
            }
        }
        MButton {
            id: returnnbtn
            text: "Return"
            width: parent.width * 0.6
            height: 64
            anchors.top: change_password_button.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: function () {
                forgotroot.visible = false
                loginroot.visible = true
            }
        }
        Text {
            id: statuss
            text: ""
            anchors.top: returnnbtn.bottom
            visible: false
            anchors.topMargin: 4
            anchors.horizontalCenter: parent.horizontalCenter
            color: "red"
            font.pointSize: 10
        }
    }

    Item {
        id: signuproot
        visible: false
        anchors.fill: parent
        Text {
            id: titleeee
            text: qsTr("Sign up")
            anchors.top: parent.top
            anchors.topMargin: 50
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 24
            font.bold: true
            color: "white"
        }

        MTextBox {
            id: name
            width: parent.width * 0.6
            height: 64
            anchors.top: titleeee.bottom
            anchors.topMargin: 60
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
        }
        Text {
            text: "Name:"
            anchors.bottom: name.top
            anchors.bottomMargin: 4
            anchors.left: name.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: surname
            width: parent.width * 0.6
            height: 64
            anchors.top: name.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
        }
        Text {
            text: "Last Name:"
            anchors.bottom: surname.top
            anchors.bottomMargin: 4
            anchors.left: surname.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: phone_number
            width: parent.width * 0.6
            height: 64
            anchors.top: surname.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
        }
        Text {
            text: "Phone Number:"
            anchors.bottom: phone_number.top
            anchors.bottomMargin: 4
            anchors.left: phone_number.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: email
            width: parent.width * 0.6
            height: 64
            anchors.top: phone_number.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            text: ""
        }
        Text {
            text: "Email address:"
            anchors.bottom: email.top
            anchors.bottomMargin: 4
            anchors.left: email.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: usernamee
            width: parent.width * 0.6
            height: 64
            anchors.top: email.bottom
            anchors.topMargin: 60
            anchors.horizontalCenter: parent.horizontalCenter
            //r text: ""
        }
        Text {
            text: "Username"
            anchors.bottom: usernamee.top
            anchors.bottomMargin: 4
            anchors.left: usernamee.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MTextBox {
            id: passwordd
            width: parent.width * 0.6
            height: 64
            anchors.top: usernamee.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            // text: ""
            echoMode: TextInput.Password
        }
        Text {
            text: "Password"
            anchors.bottom: passwordd.top
            anchors.bottomMargin: 4
            anchors.left: passwordd.left
            color: "#cccccc"
            font.pointSize: 10
        }

        MButton {
            id: sign_up_btn
            text: "Sign up"
            width: parent.width * 0.6
            height: 64
            anchors.top: passwordd.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: function () {
                if (server.sign_up(name.text, surname.text, email.text,
                                   phone_number.text, usernamee.text,
                                   passwordd.text)) {
                    statusss.visible = true
                    statusss.color = "green"
                    statusss.text = "User \"" + usernamee.text
                            + "\"\nhas been successfully registered. \nYou can login now."
                } else {
                    statusss.visible = true
                    statusss.color = "red"
                    statusss.text = server.get_server_message(1)
                }
            }
        }
        MButton {
            id: returnbtn
            text: "Return"
            width: parent.width * 0.6
            height: 64
            anchors.top: sign_up_btn.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: function () {
                signuproot.visible = false
                loginroot.visible = true
                root.onreturn()
            }
        }
        Text {
            id: statusss
            text: ""
            anchors.top: returnbtn.bottom
            visible: false
            anchors.topMargin: 4
            anchors.horizontalCenter: parent.horizontalCenter
            color: "red"
            font.pointSize: 10
        }
    }
}
