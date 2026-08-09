#include "EngineController.h"
#include "MpvObject.h"

#include <chrono>

using namespace velnix::core;

namespace {

QString stateToString(EngineState state)
{
    switch (state) {
    case EngineState::Uninitialized: return "Uninitialized";
    case EngineState::Initializing:  return "Initializing";
    case EngineState::Idle:          return "Idle";
    case EngineState::Loading:       return "Loading";
    case EngineState::Ready:         return "Ready";
    case EngineState::Playing:       return "Playing";
    case EngineState::Paused:        return "Paused";
    case EngineState::Seeking:       return "Seeking";
    case EngineState::Buffering:     return "Buffering";
    case EngineState::Stopped:       return "Stopped";
    case EngineState::Closing:       return "Closing";
    case EngineState::Shutdown:      return "Shutdown";
    }
    return "Unknown";
}

}

EngineController::EngineController(QObject *parent)
    : QObject(parent)
{
    m_engine.Initialize();
}

EngineController::~EngineController()
{
    m_engine.Shutdown();
}

void EngineController::attachBackend(MpvObject *backend)
{
    m_backend = backend;
    m_engine.SetBackend(backend);
}

void EngineController::reportIfFailed(const EngineResult &result)
{
    if (!result)
        emit engineError(QString::fromStdString(result.Message()));
    emit stateChanged();
}

void EngineController::open(const QString &path)
{
    reportIfFailed(m_engine.Open(Path(path.toStdString())));
}

void EngineController::play()
{
    reportIfFailed(m_engine.Play());
}

void EngineController::pause()
{
    reportIfFailed(m_engine.Pause());
}

void EngineController::togglePause()
{
    if (m_engine.IsPlaying())
        pause();
    else
        play();
}

void EngineController::stop()
{
    reportIfFailed(m_engine.Stop());
}

void EngineController::seek(double seconds)
{
    auto ms = std::chrono::duration_cast<Milliseconds>(std::chrono::duration<double>(seconds));
    reportIfFailed(m_engine.Seek(ms));
}

QString EngineController::state() const
{
    return stateToString(m_engine.GetState());
}
