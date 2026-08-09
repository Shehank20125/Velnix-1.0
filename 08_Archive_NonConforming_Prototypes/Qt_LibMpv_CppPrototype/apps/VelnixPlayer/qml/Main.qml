import QtQuick
import QtQuick.Controls
import VelnixPlayer 1.0

ApplicationWindow {
    id: window
    width: 1280
    height: 720
    visible: true
    title: mpv.mediaTitle.length > 0 ? mpv.mediaTitle : "Velnix Player"
    color: "#0a0a0f"

    MpvObject {
        id: mpv
        anchors.fill: parent
    }

    // Drag-and-drop a file onto the window to play it — quick way to test
    // without wiring up a file dialog yet.
    DropArea {
        anchors.fill: parent
        onDropped: (drop) => {
            if (drop.hasUrls)
                mpv.loadFile(drop.urls[0])
        }
    }

    // Minimal proof-of-life control bar. This gets replaced by the
    // auto-hiding, styled control bar from the design pass — this version
    // just proves position/duration/pause bindings work end to end.
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 64
        color: "#0f0f14"
        opacity: 0.9

        Row {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 12

            Button {
                text: mpv.pause ? "Play" : "Pause"
                onClicked: mpv.pause = !mpv.pause
            }

            Slider {
                id: seekSlider
                width: parent.width - 220
                anchors.verticalCenter: parent.verticalCenter
                from: 0
                to: mpv.duration > 0 ? mpv.duration : 1
                value: mpv.position
                onMoved: mpv.position = value
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                color: "#e4e4ea"
                text: {
                    const pos = Math.floor(mpv.position)
                    const dur = Math.floor(mpv.duration)
                    const fmt = (s) => {
                        const m = Math.floor(s / 60)
                        const sec = s % 60
                        return m + ":" + (sec < 10 ? "0" : "") + sec
                    }
                    return fmt(pos) + " / " + fmt(dur)
                }
            }
        }
    }

    Text {
        anchors.centerIn: parent
        visible: mpv.mediaTitle.length === 0
        color: "#6b6b76"
        text: "Drop a video file here to play"
        font.pixelSize: 16
    }
}
