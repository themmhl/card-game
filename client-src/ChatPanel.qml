import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Effects
import QtQuick.Shapes

Item {
    id: root
    width: 400; height: 600

    property string message: inputBox.text

    property var send_clicked:function(){}

    Connections {
        target: server

        function onChat_received(content,isMine) {
            var bubble = bubbleComponent.createObject(contentColumn, {
                text: content,
                mine: isMine,
                anchors: {
                    right: isMine ? parent.right : undefined,
                    left: isMine ? undefined : parent.left,
                    rightMargin: 8,
                    leftMargin: 8
                }
            })
            Qt.callLater(function() {
                scrollView.contentY = contentColumn.height - scrollView.height
            })
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#dd494949"
        radius: 20
        RectangularShadow {
            anchors.fill: parent
            offset.x: 0
            offset.y: 0
            radius: parent.radius
            blur: 10
            spread: 10
            color: Qt.lighter(parent.color, 1.2)
        }
        ScrollView {
            id: scrollView
            anchors {
                top: parent.top; left: parent.left; right: parent.right; bottom: emojiBar.top;
            }
            clip: true
            ScrollBar.vertical: ScrollBar {}

            Column {
                anchors.topMargin: 10
                id: contentColumn
                width: parent.width
                spacing: 10
                /*ectangle{
                    //spacer
                    height: 8
                    width: 2
                    color:"transparent"
                }*/
            }
        }

        Row {
            id: emojiBar
            anchors {
                left: parent.left; right: parent.right; bottom: inputRow.top
            }
            spacing: 4; padding: 4
            Repeater {
                model: ["😊", "😂", "👍", "🎉", "❤️"]
                delegate: Button {
                    text: modelData; font.pixelSize: 20
                    flat: true
                    onClicked: inputBox.insert(inputBox.cursorPosition, text)
                }
            }
        }

        RowLayout {
            id: inputRow
            anchors {
                left: parent.left; right: parent.right; bottom: parent.bottom;bottomMargin: 8;
            }
            spacing: 8

            MTextBox {
                id: inputBox
                Layout.fillWidth: true
                height: 48
                fontSize:20
            }

            MButton {
                text: "Send"
                height: 48
                width: 80
                onClicked: function(){
                    if (inputBox.text.length > 0) {
                        root.send_clicked()
                        inputBox.text = ""
                    }
                }
            }
        }
    }

    Component {
        id: bubbleComponent
        Item {
            width: contentColumn.width
            height: bubbleRect.height + 4

            property alias text: bubbleText.text
            property bool mine: false

            Rectangle {
                id: bubbleRect
                anchors {
                    right: mine ? parent.right : undefined
                    left: mine ? undefined : parent.left
                    rightMargin: 8
                    leftMargin: 8
                }
                width: bubbleText.implicitWidth + 24
                height: bubbleText.implicitHeight + 16
                implicitWidth: Math.min(bubbleText.implicitWidth + 24, parent.width * 0.75)


                color: mine ? "#3daee9" : "#e5e5ea"
                radius: 25
                RectangularShadow {
                    anchors.fill: parent
                    offset.x: 0
                    offset.y: 0
                    radius: parent.radius
                    blur: 5
                    spread: 5
                    color: Qt.lighter(parent.color, 1.2)
                }
                Text {
                    id: bubbleText
                    anchors.centerIn: parent
                    text: "default text"
                    wrapMode: Text.Wrap
                    font.pointSize: 20
                    width: parent.width - 24
                    color: mine ? "white" : "black"
                }
            }
        }
    }
}