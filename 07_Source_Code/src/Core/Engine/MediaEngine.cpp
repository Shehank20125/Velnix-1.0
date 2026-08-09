#include "MediaEngine.h"
#include "../Commands/Command.h"
#include "../Commands/CommandQueue.h"
#include "../Commands/CommandType.h"
#include "../Error/EngineError.h"
#include "../Events/Event.h"
#include "../Events/EventDispatcher.h"
#include "../Events/EventType.h"
#include <memory>
namespace velnix::core{
MediaEngine::MediaEngine()=default;
MediaEngine::~MediaEngine(){Shutdown();}
EngineResult MediaEngine::QueueCommand(const Command& c){if(!m_commands)return EngineResult::Failure(EngineError::NotInitialized,"Command queue has not been initialized."); if(m_state==EngineState::Shutdown)return EngineResult::Failure(EngineError::InvalidState,"Engine is shutting down."); m_commands->Push(c); return EngineResult::Success();}
EngineResult MediaEngine::Initialize(){ if(m_state!=EngineState::Uninitialized) return EngineResult::Failure(EngineError::AlreadyInitialized,"Already initialized."); m_state=EngineState::Initializing; m_commands=std::make_unique<CommandQueue>(); m_events=std::make_unique<EventDispatcher>(); m_state=EngineState::Idle; m_events->Dispatch(Event{EventType::EngineInitialized,0,"Engine initialized."}); return EngineResult::Success();}
EngineResult MediaEngine::Shutdown(){ if(m_state==EngineState::Shutdown) return EngineResult::Success(); if(m_events) m_events->Dispatch(Event{EventType::EngineShutdown,0,"Engine shutting down."}); if(m_commands)m_commands->Clear(); m_commands.reset(); m_events.reset(); m_threads.reset(); m_session.reset(); m_backend=nullptr; m_state=EngineState::Shutdown; return EngineResult::Success();}
EngineResult MediaEngine::Open(const Path& p){ if(m_state!=EngineState::Idle) return EngineResult::Failure(EngineError::InvalidState,"Invalid state."); if(!m_backend) return EngineResult::Failure(EngineError::NotInitialized,"No playback backend attached."); Command c{}; c.Type=CommandType::Open; c.File=p; auto r=QueueCommand(c); if(!r)return r; m_backend->Open(p); m_state=EngineState::Loading; if(m_events) m_events->Dispatch(Event{EventType::MediaOpened,0,p.string()}); return EngineResult::Success();}
EngineResult MediaEngine::Close(){ if(!m_backend) return EngineResult::Failure(EngineError::NotInitialized,"No playback backend attached."); Command c{}; c.Type=CommandType::Close; auto r=QueueCommand(c); if(!r)return r; m_backend->Close(); m_state=EngineState::Idle; if(m_events) m_events->Dispatch(Event{EventType::MediaClosed,0,""}); return EngineResult::Success();}
EngineResult MediaEngine::Play(){ if(!m_backend) return EngineResult::Failure(EngineError::NotInitialized,"No playback backend attached."); Command c{}; c.Type=CommandType::Play; auto r=QueueCommand(c); if(!r)return r; m_backend->Play(); m_state=EngineState::Playing; if(m_events) m_events->Dispatch(Event{EventType::PlaybackStarted,0,""}); return EngineResult::Success();}
EngineResult MediaEngine::Pause(){ if(!m_backend) return EngineResult::Failure(EngineError::NotInitialized,"No playback backend attached."); Command c{}; c.Type=CommandType::Pause; auto r=QueueCommand(c); if(!r)return r; m_backend->Pause(); m_state=EngineState::Paused; if(m_events) m_events->Dispatch(Event{EventType::PlaybackPaused,0,""}); return EngineResult::Success();}
EngineResult MediaEngine::Stop(){ if(!m_backend) return EngineResult::Failure(EngineError::NotInitialized,"No playback backend attached."); Command c{}; c.Type=CommandType::Stop; auto r=QueueCommand(c); if(!r)return r; m_backend->Stop(); m_state=EngineState::Stopped; if(m_events) m_events->Dispatch(Event{EventType::PlaybackStopped,0,""}); return EngineResult::Success();}
EngineResult MediaEngine::Seek(Milliseconds pos){ if(!m_backend) return EngineResult::Failure(EngineError::NotInitialized,"No playback backend attached."); Command c{}; c.Type=CommandType::Seek; c.Position=pos; auto r=QueueCommand(c); if(!r)return r; m_backend->Seek(pos); m_state=EngineState::Seeking; if(m_events) m_events->Dispatch(Event{EventType::SeekStarted,0,""}); return EngineResult::Success();}
EngineState MediaEngine::GetState() const noexcept{return m_state;}
bool MediaEngine::IsInitialized() const noexcept{return m_state!=EngineState::Uninitialized&&m_state!=EngineState::Shutdown;}
bool MediaEngine::IsPlaying() const noexcept{return m_state==EngineState::Playing;}
bool MediaEngine::IsPaused() const noexcept{return m_state==EngineState::Paused;}
}
