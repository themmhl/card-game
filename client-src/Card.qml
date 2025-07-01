import QtQuick
import QtQuick.Effects
import QtQuick.Shapes

Item {
    id: root
    property int suit: 0
    property int rank: 0
    property int card_id: 0
    property real hue: 0.5
    property real currentAlpha: 0.8
    property real currentHue: 0.9
    property real shadowBlur: 5
    property real shadowSpread: 2
    property real shadowAlpha: 0.2
    property color initial_color: Qt.hsva(0.9, 0, 1, 0.2)
    property var onClicked: function () {}
    property var onHover: function () {}
    property var onLeave: function () {}
    property real color_opacity: 0.5
    function get_path() {
        //sample:   ://card-pics/Dimond-10.JPG
        var path = "qrc:/card-pics/"

        switch (suit) {
        case 0:
            path += "Dimond-"
            break
        case 1:
            path += "Dollar-"
            break
        case 2:
            path += "Coin-"
            break
        case 3:
            path += "Gold-"
            break
        }

        switch (rank) {
        case 0:
            path += "2"
            break
        case 1:
            path += "3"
            break
        case 2:
            path += "4"
            break
        case 3:
            path += "5"
            break
        case 4:
            path += "6"
            break
        case 5:
            path += "7"
            break
        case 6:
            path += "8"
            break
        case 7:
            path += "9"
            break
        case 8:
            path += "10"
            break
        case 9:
            path += "Solider"
            break
        case 10:
            path += "Queen"
            break
        case 11:
            path += "King"
            break
        case 12:
            path += "Bitcoin"
            break
        }
        path += ".JPG"
        return path
    }
    Rectangle {
        id: rootrect
        anchors.fill: parent
        color: "transparent"
        //pic here
        Image {
            id: img
            source: root.get_path()
            anchors.fill: parent
            visible: false
        }
        MultiEffect {
            antialiasing: true
            id: effect
            source: img
            colorizationColor: Qt.hsva(root.currentHue, 1, 0.5,
                                       root.currentAlpha)
            colorization: 0
            anchors.fill: img
            brightness: 0
            visible: true
            maskEnabled: true
            maskSource: mask
            z: 4
        }
        Item {
            antialiasing: true
            id: mask
            anchors.fill: img
            layer.enabled: true
            visible: false
            Rectangle {
                antialiasing: true
                anchors.fill: parent
                radius: 25
                color: "black"
            }
        }

        PropertyAnimation {
            id: mousein
            target: effect
            properties: "colorization"
            to: 0.6
            duration: 150
            onStarted: {
                mouseincolor.running = true
                //effect.colorization = 0.5
                effect.brightness = 0.2
                shadow.color.hslSaturation = 1
            }
        }

        SequentialAnimation {
            id: mouseincolor
            PropertyAnimation {
                target: root
                property: "currentHue"
                from: 0.9
                to: 0.5
                duration: 1250
            }
            PropertyAnimation {
                target: root
                property: "currentHue"
                from: 0.5
                to: 0.9
                duration: 1250
            }
            loops: Animation.Infinite
        }

        PropertyAnimation {
            id: mouseout
            target: effect
            properties: "colorization,brightness"
            to: 0
            duration: 150
            onFinished: {
                mouseincolor.running = false
                // effect.colorization = 0
                // effect.brightness = 0
                shadow.color.hslSaturation = 0
            }
        }

        PropertyAnimation {
            id: mousedown
            target: root
            property: "currentAlpha"
            from: 0.8
            to: 0.4
            duration: 150
        }

        PropertyAnimation {
            id: mouseup
            target: root
            property: "currentAlpha"
            from: 0.4
            to: 0.8
            duration: 200
        }

        RectangularShadow {

            id: shadow
            anchors.fill: rootrect
            offset.x: 0
            offset.y: 0
            radius: 25
            blur: root.shadowBlur
            spread: 10
            color: effect.colorizationColor
            z: -1
            Component.onCompleted: {
                shadow.color.hslSaturation = 0
            }
        }

        PropertyAnimation {
            id: shadowIn
            target: root
            properties: "shadowBlur,shadowSpread"
            to: 20
            duration: 150
        }

        PropertyAnimation {
            id: shadowOut
            target: root
            properties: "shadowBlur,shadowSpread"
            to: 10
            duration: 150
        }

        MouseArea {
            z: 10
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            anchors.fill: parent
            onEntered: {
                root.onHover()
                mousein.start()
                shadowIn.start()
            }
            onExited: {
                root.onLeave()
                mouseout.start()
                shadowOut.start()
            }
            onPressed: mousedown.start()
            onReleased: {
                mouseup.start()
                root.onClicked()
            }
        }
    }
}
