import QtQuick
import QtQuick.Effects
import QtQuick.Shapes


Item {
    id: root
    property string text: "edfg"
    property color initial_color: Qt.hsva(0, 0, 1, 0.2)
    property var onClicked: function () {}
    property real color_opacity: 0.2
    Rectangle {
        id: rootrect
        anchors.fill: parent
        color: root.initial_color

        radius: height / 2 //material you design guidelines
        Text {
            id: name
            text: qsTr(root.text)
            anchors.centerIn: parent
            color: "white"
            font.pointSize: 14
            font.bold: true
            z: 2
        }

        PropertyAnimation {
            id: mousein
            target: rootrect
            property: "color.a"
            from: root.color_opacity
            to: root.color_opacity + 0.3
            duration: 150
            onStarted: {
                mouseincolor.running = true
                rootrect.color.hsvSaturation = 0.7
            }
        }
        SequentialAnimation {
            id: mouseincolor
            PropertyAnimation {
                target: rootrect
                property: "color.hsvHue"
                from: 0.5
                to: 0.9
                duration: 1250
            }
            PropertyAnimation {
                target: rootrect
                property: "color.hsvHue"
                from: 0.9
                to: 0.5
                duration: 1250
            }
            loops: Animation.Infinite
        }
        PropertyAnimation {
            id: mouseout
            target: rootrect
            properties: "color.a, color.hsvSaturation"
            from: root.color_opacity + 0.3
            to: root.color_opacity
            duration: 150
            onFinished: {
                mouseincolor.running = false
                rootrect.color.hsvSaturation = 0
            }
        }

        RectangularShadow {
            anchors.fill: rootrect
            offset.x: 0
            offset.y: 0
            radius: rootrect.radius
            blur: 10
            spread: 10
            color: Qt.lighter(rootrect.color, 1.2)
        }

        Shape {
            id: shape
            property double radius: 0.5
            anchors.fill: parent
            visible: false
            //antialiasing: true
            ShapePath {
                strokeWidth: 0
                fillGradient: RadialGradient {
                    id: fillgrad
                    centerRadius: 150
                    focalX: centerX
                    focalY: centerY
                    GradientStop {
                        position: shape.radius
                        color: Qt.lighter(rootrect.color, 2)
                    }
                    GradientStop {
                        position: shape.radius + 0.01
                        color: rootrect.color
                    }
                }
                startX: 0
                startY: root.height / 2
                PathArc {
                    x: root.height / 2
                    y: 0
                    radiusX: root.height / 2
                    radiusY: root.height / 2
                }
                PathLine {
                    x: root.width - root.height / 2
                    y: 0
                }

                PathArc {
                    x: root.width
                    y: root.height / 2
                    radiusX: root.height / 2
                    radiusY: root.height / 2
                }

                PathArc {
                    x: root.width - root.height / 2
                    y: root.height
                    radiusX: root.height / 2
                    radiusY: root.height / 2
                }

                PathLine {
                    x: root.height / 2
                    y: root.height
                }
                PathArc {
                    x: 0
                    y: root.height / 2
                    radiusX: root.height / 2
                    radiusY: root.height / 2
                }
            }
        }
        PropertyAnimation {
            id: mouseup
            target: shape
            property: "opacity"
            from: 1
            to: 0
            duration: 100
            onFinished: () => {
                            shape.radius = 0
                            shape.visible = false
                            shape.opacity = 1
                            //so you see the beautiful animation before anything
                            root.onClicked()
                        }
        }
        PropertyAnimation {
            id: gradanimation
            target: shape
            property: "radius"
            from: 0.01
            to: 1
            duration: 200
        }

        MouseArea {
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            anchors.fill: parent
            onEntered: mousein.start()
            onExited: mouseout.start()
            onPressed: () => {
                           mouseup.stop()
                           gradanimation.stop()
                           fillgrad.centerX = mouseX
                           fillgrad.centerY = mouseY
                           fillgrad.focalX = mouseX
                           fillgrad.focalY = mouseY
                           fillgrad.centerRadius
                           = (root.width - mouseX > mouseX) ? root.width - mouseX : mouseX
                           shape.visible = true
                           gradanimation.start()
                       }
            onReleased: {
                mouseup.start()
            }
        }
    }
}
