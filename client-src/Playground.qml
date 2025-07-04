import QtQuick
import QtQuick.Dialogs

Item {
    id: root
    anchors.fill: parent
    property string username: "Lil mane mane"
    property string opp_username: "Nigga creep"
    property int my_score: 0
    property int opp_score: 0
    property real countdown: 10
    property bool pause_resume: false
    property int idle_count: 0
    property var onReturn: function () {
    }
    property bool flip:false

    Connections {
        target: server
        function onFlip_requested(){
            status.color = "#b39e9e9e"
            status.visible = true
            status.status_text = "You opponent requested\nA Flip\nClick on Flip button to accept"
            status.title_text = "Flip Request"
            status.opacity = 1
            pause.running = true
        }
        function onFlip_request_accepted(){
            status.color = "#b376c260"
            status.visible = true
            status.status_text = "You can choose an\nopponent's card"
            status.title_text = "Flip"
            status.opacity = 1
            pause.running = true
            opp_selected.select_enabled = true
            flip = true
        }
        function onStart_timer() {
            countdown = 10;
            root.pause_resume = false
            countdownTimer.stop();
            delayTimer.start();
        }

        function onStop_timer() {
            countdownTimer.stop();
            delayTimer.stop();
            countdown_text.visible = false;
            countdown = 10;
        }

        function onAdd_card(suit, rank, where) {
            //if(!idle.running) idle.running = true;

            //root.username = server.getPlayerName()+ " " +server.getPlayerSurname()
            //make sure to clear them first
            //where : MY_SELECTED, MY_AVAILABLE, OPP_SELECTED, OPP_AVAILABLE
            switch (where) {
                case 0:
                    my_selected.add_card(suit, rank)
                    break
                case 1:
                    //its my turn
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

            countdown = 10;
            countdownTimer.stop();
            delayTimer.stop();
            countdown_text.visible = false;
            opp_available.clearCards()
            opp_selected.clearCards()
            my_selected.clearCards()
            my_available.clearCards()
            username = server.getPlayerUsername()
            opp_username = server.getOppUsername()

            my_score = server.get_score(0)
            opp_score = server.get_score(1)
        }

        function onShow_status(title, context, color) {
            status.color = color
            status.visible = true
            status.status_text = context
            status.title_text = title
            status.opacity = 1
            pause.running = true
        }

        function onGame_over(title, context, color) {
            if (pause.running) pause.stop()
            countdown = 10;
            countdownTimer.stop();
            delayTimer.stop();
            countdown_text.visible = false;
            status.color = color
            status.visible = true
            status.status_text = context
            status.title_text = title
            status.opacity = 1
            returnbtn.visible = true
        }
    }
    Timer {
        id: delayTimer
        interval: 20000
        repeat: false
        onTriggered: {
            root.countdown = 10;
            countdown_text.text = root.countdown.toString();
            countdown_text.visible = true;
            countdownTimer.start();
        }
    }

    Timer {
        id: countdownTimer
        property bool pause_resume: false
        interval: 1000
        repeat: true
        running: false
        onTriggered: {
            root.countdown--;
            countdown_text.text = root.countdown.toString();

            if (root.countdown <= 0) {
                countdownTimer.stop();

                if (++root.idle_count === 2 || root.pause_resume) {
                    // You lost
                    status.color = "#b3ff0000"
                    my_available.select_enabled = false
                    status.visible = true
                    status.status_text = "Your time to choose a card has exceeded for the second time"
                    status.title_text = "You Lost."
                    status.opacity = 1
                    returnbtn.visible = true
                    root.pause_resume = false
                    server.timeout_lose()
                } else {
                    // First timeout
                    status.color = "#b39e9e9e"
                    status.visible = true
                    status.status_text = "A random card has been chosen.\nNext time you'll lose."
                    status.title_text = "Time's Up!"
                    status.opacity = 1
                    pause.running = true

                    // Pick random card
                    let cards = my_available.cards;
                    if (cards.length > 0) {
                        let randomIndex = Math.floor(Math.random() * cards.length);
                        let randomCard = cards[randomIndex];
                        server.select_card(randomCard.suit, randomCard.rank);
                    }
                }

                // Score update
                my_score = server.get_score(0)
                opp_score = server.get_score(1)
            }
        }
    }

    PauseAnimation {
        id: pause
        duration: 4000
        onFinished: {
            //status.visible = false
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
                if (chat.opacity === 1)
                    chat.opacity = 0
                else
                    chat.opacity = 1
            }
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20
            y: 10
            MButton {
                text: "Stop"
                enabled: root.my_available._id !== 0
                height: 54
                width: 180
                onClicked: function () {
                    if (text === "Stop") {
                        text = "Resume"
                        root.countdown = 20
                        countdown_text.text = root.countdown.toString();
                        countdown_text.visible = true;
                        root.pause_resume = true
                        countdownTimer.start();
                    }
                    if (text === "Resume") {
                        text = "Stop"
                    }
                }
            }
            MButton {
                text: "Flip"
                height: 54
                width: 180

                onClicked:function () {
                    server.flip_request()
                    status.color = "#b39e9e9e"
                    status.visible = true
                    status.status_text = "You requested A Flip\nWait for your opponent."
                    status.title_text = "Flip Requested"
                    status.opacity = 1
                    pause.running = true
                }
            }
        }
        MButton {
            text: "Exit"
            anchors.right: parent.right
            y: 10
            anchors.rightMargin: 10
            height: 54
            width: 180
            onClicked: function () {
                // You lost
                status.color = "#b3ff0000"
                my_available.select_enabled = false
                status.visible = true
                status.status_text = "Your chose to quit."
                status.title_text = "You Lost."
                status.opacity = 1
                returnbtn.visible = true
                server.timeout_lose()
            }
        }
    }
    MessageDialog {
        id: exitDialog
        title: "Confirm Quit"
        text: "Are you sure you want to quit? You'll lose."
        buttons: MessageDialog.Yes | MessageDialog.No
        onAccepted: {
            //we dont need to check for anything bc either way we have lost.
            my_available.select_enabled = false
            server.exit_request();
            status.color = "#b3ff0000"
            status.visible = true
            status.status_text = "You chose to surrender."
            status.title_text = "You Lost."
            status.opacity = 1
            returnbtn.visible = true
            my_score = server.get_score(0)
            opp_score = server.get_score(1)
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
        flip:true
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
                    text: root.opp_username.charAt(0)
                    font.pointSize: 24
                    anchors.centerIn: parent
                    color: "white"
                }
            }
            Text {
                id: text_opp
                text: root.opp_username
                anchors.verticalCenter: parent.verticalCenter
                font.family: "Noto Sans"

                font.pixelSize: 32
                color: "black"
            }
            Rectangle {
                height: 10
                opacity: 0
                //spacer
                width: 300 - 60 - text_user.width
            }
            Text {
                text: root.opp_score
                anchors.verticalCenter: parent.verticalCenter
                font.family: "Noto Sans"

                font.pixelSize: 40
                color: "black"
            }
        }
        Rectangle{
            width: root.width
            height: 3
            color: "darkred"
        }
        Row {
            spacing: 8
            Rectangle {
                height: 60
                width: 60
                radius: 30
                color: "#00b7d3"
                Text {
                    text: root.username.charAt(0)
                    font.pointSize: 24
                    anchors.centerIn: parent
                    color: "white"
                }
            }
            Text {
                id: text_user
                text: root.username
                anchors.verticalCenter: parent.verticalCenter
                font.family: "Noto Sans"
                font.pixelSize: 32
                color: "black"
            }
            Rectangle {
                height: 10
                opacity: 0
                //spacer
                width: 300 - 60 - text_opp.width
            }
            Text {
                text: root.my_score
                anchors.verticalCenter: parent.verticalCenter
                font.family: "Noto Sans"

                font.pixelSize: 40
                color: "black"
            }
        }
    }
    Rectangle {
        id: status
        anchors.fill: parent
        color: "#b3a3a3a3"
        visible: opacity === 0
        property string title_text: "You Lost."
        property string status_text: "maybe winning pattern?"
        opacity: 0
        Behavior on opacity {
            NumberAnimation {
                duration: 500
            }
        }
        Column {
            anchors.centerIn: parent
            Text {
                text: status.title_text
                font.pointSize: 80
                font.bold: true
                color: "black"
            }
            Text {
                text: status.status_text
                font.pointSize: 50
                color: "#141414"
            }
        }
    }
    MButton {
        id: returnbtn
        visible: false
        text: "Return"
        y: 500
        anchors.horizontalCenter: parent.horizontalCenter
        height: 60
        width: 180
        z: 10
        onClicked: function () {
            username = server.getPlayerUsername()
            opp_username = ""
            status.visible = false
            returnbtn.visible = false
            my_score = 0
            opp_score = 0
            root.onReturn()
        }
    }
    Text {
        id: countdown_text
        text: root.countdown
        anchors.centerIn: parent
        visible: false
        font.bold: true
        font.pointSize: 200

        color: "#80910000"
    }


    ChatPanel {
        id: chat
        x: 5
        y: 75
        visible: opacity !== 0
        opacity: 0
        Behavior on opacity {
            NumberAnimation {
                duration: 500
            }
        }
        send_clicked: function(){
            server.send_chat(message);
        }
    }
}