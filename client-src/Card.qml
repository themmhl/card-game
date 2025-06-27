import QtQuick
import QtQuick.Effects

Item {
    id: root
    property string imgPath: ""
    property real hue: 0.5
    width: 305
    height: 400
    Image {
        id: img
        source: "qrc:/coin.jpg"
        anchors.fill: parent
        visible: false
    }
    MultiEffect {
        antialiasing: true
        id: effect
        source: img
        colorizationColor: Qt.hsva(root.hue, 1, 0.5, 1)
        colorization: 0.5
        anchors.fill: img
        brightness: 0.5
        visible: true
        maskEnabled: true
        maskSource: mask
        z: 2
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
    RectangularShadow {
        antialiasing: true
        anchors.fill: root
        offset.x: 0
        offset.y: 0
        radius: 25
        blur: 20
        spread: 20
        color: Qt.lighter(effect.colorizationColor, 1.1)
    }

    SequentialAnimation {
        id: mouseincolor
        PropertyAnimation {
            target: root
            property: "hue"
            from: 0.5
            to: 0.9
            duration: 1250
        }
        PropertyAnimation {
            target: root
            property: "hue"
            from: 0.9
            to: 0.5
            duration: 1250
        }
        loops: Animation.Infinite
        running: true
    }
}
