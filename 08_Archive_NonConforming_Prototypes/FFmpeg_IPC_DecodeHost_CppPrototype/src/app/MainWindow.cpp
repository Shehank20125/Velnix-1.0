#include "MainWindow.h"

namespace mpe {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ipcChannel_(std::make_unique<StubIpcChannel>()),
      frameSink_(std::make_unique<NullFrameSink>()) {
    setWindowTitle("Media Player — Core Engine (scaffold)");
    resize(1280, 720);

    // TODO: build video surface widget + transport controls, wire
    // ipcChannel_->setMessageHandler(...) to route incoming frame-ready
    // messages to frameSink_->submitFrame(...).
}

MainWindow::~MainWindow() = default;

} // namespace mpe
