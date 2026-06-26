import QtQuick
import QtQuick.Particles
import QtQuick.Controls

Window {
    id: root
    width: 520
    height: 480
    minimumWidth: 400
    minimumHeight: 350
    visible: true
    title: "Qt6 on QNX 8.0"
    color: "#050510"

    property real h: 0
    property real p: 0
    property real w: 0
    property real r: 0
    property real s: 0
    property real g: 0
    NumberAnimation on h { from: 0; to: 1; duration: 50000; loops: Animation.Infinite }
    NumberAnimation on p { from: 0; to: 1; duration: 2500; loops: Animation.Infinite; easing.type: Easing.InOutSine }
    NumberAnimation on w { from: 0; to: 1; duration: 7000; loops: Animation.Infinite }
    NumberAnimation on r { from: 0; to: 1; duration: 18000; loops: Animation.Infinite }
    NumberAnimation on s { from: 0; to: 1; duration: 9000; loops: Animation.Infinite; easing.type: Easing.InOutSine }
    NumberAnimation on g { from: 0; to: 1; duration: 5000; loops: Animation.Infinite; easing.type: Easing.InOutSine }

    readonly property color bc0: Qt.hsla(0.58 + h*0.5, 0.7, 0.045 + 0.025*Math.sin(h*7), 1)
    readonly property color bc1: Qt.hsla(0.68 + h*0.5, 0.6, 0.04 + 0.02*Math.cos(h*6), 1)
    readonly property color bc2: Qt.hsla(0.78 + h*0.5, 0.75, 0.035 + 0.02*Math.sin(h*8), 1)

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: root.bc0 }
            GradientStop { position: 0.5; color: root.bc1 }
            GradientStop { position: 1.0; color: root.bc2 }
        }
    }

    /* Aurora */
    Repeater {
        model: 8
        Rectangle {
            readonly property real bh: 0.48 + root.h*0.35 + index*0.006
            readonly property real by0: root.height * (0.02 + index*0.06)
            y: by0 + Math.sin(root.w*6.28 + index*0.9)*root.height*0.14 + Math.sin(root.w*3.14 + index*2.7)*root.height*0.06
            x: -root.width*0.2
            width: root.width*1.4
            height: root.height * (0.1 + index*0.02)
            radius: height/2
            color: Qt.hsla(bh, 0.15 + index*0.015, 0.35 + index*0.015,
                0.04 + 0.035*Math.sin(root.w*5+index*0.6))
        }
    }

    /* Outer ring jewels */
    Repeater {
        model: 12
        Item {
            readonly property real ja: index*30 + root.r*360*0.1
            readonly property real jr: 280 + 16*Math.sin(root.r*3.5 + index*0.9)
            x: root.width/2 + Math.cos(ja*Math.PI/180)*jr - 8
            y: root.height/2 + Math.sin(ja*Math.PI/180)*jr - 8

            Rectangle {
                width: 14 + 8*Math.sin(root.r*3 + index*1.3)
                height: width; radius: width/2
                color: Qt.hsla(0.58 + root.h*0.25 + index*0.015, 0.7, 0.72,
                    0.5 + 0.3*Math.sin(root.r*5 + index*0.7))
                Rectangle {
                    anchors.centerIn: parent
                    width: parent.width + 16
                    height: width; radius: width/2
                    color: "transparent"
                    border.color: parent.color
                    border.width: 2; opacity: 0.3
                }
            }
        }
    }

    /* Inner ring jewels */
    Repeater {
        model: 8
        Item {
            readonly property real ja: index*45 - root.r*360*0.16
            readonly property real jr: 170 + 12*Math.cos(root.r*3 + index*1.1)
            x: root.width/2 + Math.cos(ja*Math.PI/180)*jr - 6
            y: root.height/2 + Math.sin(ja*Math.PI/180)*jr - 6

            Rectangle {
                width: 10 + 6*Math.cos(root.r*4 + index*2)
                height: width; radius: width/2
                color: Qt.hsla(0.68 + root.h*0.2 + index*0.025, 0.75, 0.8,
                    0.45 + 0.3*Math.sin(root.r*4.5 + index*0.6))
            }
            Rectangle {
                x: -4; y: -4
                width: parent.width + 8; height: width; radius: width/2
                color: "transparent"
                border.color: Qt.hsla(0.68 + root.h*0.2 + index*0.025, 0.5, 0.6, 0.15)
                border.width: 1
            }
        }
    }

    /* Middle diamonds */
    Repeater {
        model: 6
        Item {
            readonly property real da: index*60 + root.r*360*0.13
            x: root.width/2 + Math.cos(da*Math.PI/180)*230 - 20
            y: root.height/2 + Math.sin(da*Math.PI/180)*230 - 20

            Rectangle {
                width: 30 + 18*Math.sin(root.r*3.5 + index*1.8)
                height: width
                rotation: 45 + root.r*360*0.08
                color: Qt.hsla(0.62 + root.h*0.22 + index*0.04, 0.55, 0.58,
                    0.1 + 0.06*Math.sin(root.r*4 + index))
                border.color: Qt.hsla(0.62 + root.h*0.22 + index*0.04, 0.65, 0.65,
                    0.15 + 0.08*Math.sin(root.r*3 + index*1.5))
                border.width: 2
            }
        }
    }

    /* Moire rings */
    Repeater {
        model: 18
        Rectangle {
            readonly property real mw: 20 + index*30
            readonly property real mh: mw * (0.9 + 0.1*Math.sin(index*1.7))
            x: root.width/2 - mw/2
            y: root.height/2 - mh/2
            width: mw; height: mh; radius: mh/2
            color: "transparent"
            border.color: Qt.hsla(0.6 + root.h*0.22 + index*0.01, 0.4, 0.5,
                0.03 + 0.025*Math.sin(root.r*4 + index*2.3))
            border.width: 2
            rotation: root.r*360*(0.05 + index*0.03)*(index%2 ? 1 : -1)
            transformOrigin: Item.Center
        }
    }

    /* Large ovals */
    Repeater {
        model: 3
        Rectangle {
            readonly property real ow: [300, 440, 560][index]
            readonly property real oh: [160, 240, 120][index]
            x: root.width/2 - ow/2
            y: root.height/2 - oh/2
            width: ow; height: oh; radius: oh/2
            color: "transparent"
            border.color: Qt.hsla(0.55 + root.h*0.22 + index*0.07, 0.55, 0.58,
                0.06 + 0.04*Math.sin(root.r*3 + index*4))
            border.width: [3, 2, 2][index]
            rotation: root.r*360*(0.15 + index*0.05)*([1, -1, 1][index])
            transformOrigin: Item.Center

            Rectangle {
                x: width/2 - 5; y: -5
                width: 10; height: 10; radius: 5
                color: Qt.hsla(0.55 + root.h*0.22 + index*0.07, 0.75, 0.78,
                    0.45 + 0.25*Math.sin(root.r*5 + index))
            }
        }
    }

    /* Radial spokes */
    Repeater {
        model: 72
        Rectangle {
            readonly property real sa: index*(360/72)
            x: root.width/2 + Math.cos(sa*Math.PI/180)*15 - 1
            y: root.height/2 + Math.sin(sa*Math.PI/180)*15 - 1
            width: 2
            height: 130 + 60*Math.sin(root.s*6.28 + index*0.5)
            color: Qt.hsla(0.6 + root.h*0.22 + index*0.004, 0.35, 0.65,
                0.03 + 0.025*Math.sin(root.r*6 + index*0.3))
            transformOrigin: Item.TopLeft
            rotation: sa + root.r*360*0.06
        }
    }

    /* Central glow */
    Rectangle {
        x: root.width/2 - 180; y: root.height/2 - 180
        width: 360; height: 360; radius: 180
        color: Qt.hsla(0.6 + root.h*0.15, 0.4, 0.55, 0.05 + 0.035*Math.sin(root.g*6.28))
    }
    Rectangle {
        x: root.width/2 - 90; y: root.height/2 - 90
        width: 180; height: 180; radius: 90
        color: Qt.hsla(0.65 + root.h*0.15, 0.5, 0.65, 0.08 + 0.05*Math.sin(root.g*5.28))
    }
    Rectangle {
        x: root.width/2 - 35; y: root.height/2 - 35
        width: 70; height: 70; radius: 35
        color: Qt.hsla(0.7 + root.h*0.1, 0.6, 0.85, 0.15 + 0.08*Math.sin(root.g*4.28))
    }

    /* Shooting stars */
    Repeater {
        model: 3
        Item {
            property real sp: 0
            NumberAnimation on sp {
                from: 0; to: 1; duration: 2500 + index*1200
                loops: Animation.Infinite
                easing.type: Easing.InSine
            }
            readonly property real sx: root.width * [0.85, 0.7, 0.9][index]
            readonly property real sy: root.height * [0.03, 0.07, 0.1][index]
            x: sx - sp*350
            y: sy + sp*180 + sp*sp*60
            opacity: (sp < 0.08 ? sp*12 : (sp > 0.85 ? (1-sp)*6 : 1)) * 0.7

            Rectangle { x: 0; y: -1; width: 3; height: 3; radius: 1.5; color: "white" }
            Rectangle { x: -8; y: 0; width: 18; height: 1.5; radius: 1; color: Qt.rgba(0.8,0.9,1,0.35) }
            Rectangle { x: 0; y: -2; width: 12; height: 5; radius: 3; color: Qt.rgba(0.6,0.8,1,0.2) }
        }
    }

    /* Starfield */
    Repeater {
        model: 120
        Rectangle {
            readonly property real sx: (Math.sin(index*5.7+3.1)*0.5+0.5)*root.width
            readonly property real sy: (Math.sin(index*3.1+7.3)*0.5+0.5)*root.height
            readonly property real sz: 0.8 + Math.sin(index*8.3+2.7)*0.6
            x: sx; y: sy
            width: sz; height: sz; radius: sz/2
            color: Qt.rgba(0.9, 0.95, 1, opacity)
            opacity: (0.2+Math.sin(index*4.1+1.3)*0.15)*(0.6+0.4*Math.sin(root.h*10+index*1.1))
        }
    }

    /* Particles */
    ParticleSystem {
        id: ps
        anchors.fill: parent
        running: visible
        Emitter {
            anchors.fill: parent
            lifeSpan: 6000
            emitRate: 18
            size: 1
            sizeVariation: 5
            velocity: PointDirection { xVariation: 100; yVariation: -140; y: -70 }
            acceleration: PointDirection { xVariation: 30; y: 12 }
        }
        Emitter {
            anchors.fill: parent
            lifeSpan: 3000
            emitRate: 8
            size: 1
            sizeVariation: 4
            velocity: PointDirection { xVariation: 80; yVariation: 80 }
            acceleration: PointDirection { xVariation: 20; y: -20 }
        }
        ItemParticle {
            delegate: Rectangle {
                width: 2+Math.random()*5
                height: width; radius: width/2
                color: Qt.hsla(root.h+0.5+Math.random()*0.3, 0.5+Math.random()*0.4, 0.6+Math.random()*0.3, 0.25+Math.random()*0.35)
            }
        }
    }

    /* UI */
    Item {
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height - 100
        width: parent.width*0.75
        height: 80

        Rectangle {
            anchors.fill: parent
            radius: 20
            color: Qt.rgba(0.035, 0.07, 0.16, 0.35)
            border.color: Qt.hsla(0.6+root.h*0.2, 0.3, 0.5, 0.06+0.04*Math.sin(root.h*8))
            border.width: 1
        }

        Column {
            anchors.centerIn: parent
            spacing: 5

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Qt6 on QNX 8.0"
                    font.pixelSize: 15; font.bold: true
                    font.letterSpacing: 1.5
                    color: "white"
                    style: Text.Raised
                    styleColor: Qt.hsla(root.h*0.8, 0.5, 0.5, 0.3)
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "·"
                    font.pixelSize: 14
                    color: Qt.hsla(root.h*0.8, 0.3, 0.5, 0.3)
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Cosmic Mandala"
                    font.pixelSize: 10
                    font.letterSpacing: 2.5
                    font.capitalization: Font.AllUppercase
                    color: Qt.hsla(root.h*0.8, 0.25, 0.55, 0.4)
                }
            }

            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 200; height: 8; radius: 4
                color: Qt.rgba(1,1,1,0.035)

                Rectangle {
                    x: 0; y: 0
                    width: parent.width*root.p
                    height: parent.height; radius: 4
                    color: Qt.hsla(0.58+root.p*0.35+root.h*0.1, 0.8, 0.55, 1)
                }
                Rectangle {
                    x: 200*root.p - 8
                    y: -4
                    width: 16; height: 16; radius: 8
                    color: Qt.hsla(0.58+root.p*0.35+root.h*0.1, 0.9, 0.8, 0.6)
                    Rectangle {
                        anchors.centerIn: parent
                        width: 24; height: 24; radius: 12
                        color: "transparent"
                        border.color: parent.color; border.width: 1.5; opacity: 0.25
                    }
                }
            }

            Button {
                id: closeBtn
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Close"
                contentItem: Text {
                    text: closeBtn.text
                    color: "white"
                    font.pixelSize: 11; font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    radius: 14
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: root.cb0 }
                        GradientStop { position: 1.0; color: root.cb1 }
                    }
                    Rectangle {
                        anchors.fill: parent; radius: 14
                        color: "transparent"
                        border.color: Qt.rgba(1,1,1,closeBtn.hovered?0.2:0.04)
                        border.width: 1
                    }
                }
                leftPadding: 24; rightPadding: 24
                topPadding: 7; bottomPadding: 7
                scale: closeBtn.hovered ? 1.08 : 1.0
                Behavior on scale { NumberAnimation { duration: 70; easing.type: Easing.OutBack } }
                onClicked: Qt.quit()
            }
        }
    }

    readonly property color cb0: closeBtn.hovered
        ? Qt.hsla(root.h*0.8, 0.65, 0.55, 1)
        : Qt.hsla(root.h*0.8, 0.45, 0.32, 0.7)
    readonly property color cb1: closeBtn.hovered
        ? Qt.hsla(root.h*0.8+0.05, 0.75, 0.45, 1)
        : Qt.hsla(root.h*0.8+0.05, 0.55, 0.28, 0.7)
}