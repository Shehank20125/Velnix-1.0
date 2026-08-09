#pragma once

#include <memory>
#include <QMainWindow>
#include "../ipc/IpcChannel.h"
#include "../render/FrameSink.h"

namespace mpe {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    // TODO: playback controls (play/pause/seek/volume), video surface
    // widget, and the synchronization logic (audio clock vs. video PTS)
    // referenced in the Phase 2 plan all live here or in helper classes
    // owned by MainWindow.

    std::unique_ptr<IpcChannel> ipcChannel_;  // currently StubIpcChannel
    std::unique_ptr<FrameSink> frameSink_;    // currently NullFrameSink
};

} // namespace mpe
