// Main application process. This process does the UI, playback control,
// and synchronization — it never links FFmpeg or touches decode logic
// directly (see Threat Model item 1: decode runs only in the Decode Host,
// a separate process spawned by this one).

#include <QApplication>
#include "MainWindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    mpe::MainWindow window;
    window.show();

    // TODO: spawn the Decode Host child process here (path resolution,
    // process lifecycle/crash-restart policy TBD) and hand MainWindow an
    // IpcChannel connected to it once the IPC transport is chosen.

    return app.exec();
}
