#include "MediaEngine.h"
#include "../Commands/Command.h"
#include "../Commands/CommandQueue.h"
#include "../Commands/CommandType.h"
#include "../Error/EngineError.h"
#include <memory>
namespace velnix::core{
MediaEngine::MediaEngine()=default;
MediaEngine::~MediaEngine(){Shutdown();}
EngineResult MediaEngine::QueueCommand(const Command& c){if(!m_commands)return EngineResult::Failure(EngineError::NotInitialized,"Command queue has not been initialized."); if(m_state==EngineState::Shutdown)return EngineResult::Failure(EngineError::InvalidState,"Engine is shutting down."); m_commands->Push(c); return EngineResult::Success();}
EngineResult MediaEngine::Initialize(){ if(m_state!=EngineState::Uninitialized) return EngineResult::Failure(EngineError::AlreadyInitialized,"Already initialized."); m_state=EngineState::Initializing; m_commands=std::make_unique<CommandQueue>(); m_state=EngineState::Idle; return EngineResult::Success();}
EngineResult MediaEngine::Shutdown(){ if(m_state==EngineState::Shutdown) return EngineResult::Success(); if(m_commands)m_commands->Clear(); m_commands.reset(); m_events.reset(); m_threads.reset(); m_session.reset(); m_state=EngineState::Shutdown; return EngineResult::Success();}
EngineResult MediaEngine::Open(const Path& p){ if(m_state!=EngineState::Idle) return EngineResult::Failure(EngineError::InvalidState,"Invalid state."); Command c{}; c.Type=CommandType::Open; c.File=p; auto r=QueueCommand(c); if(!r)return r; m_state=EngineState::Loading; return EngineResult::Success();}
EngineResult MediaEngine::Close(){ Command c{}; c.Type=CommandType::Close; auto r=QueueCommand(c); if(!r)return r; m_state=EngineState::Idle; return EngineResult::Success();}
EngineResult MediaEngine::Play(){ Command c{}; c.Type=CommandType::Play; auto r=QueueCommand(c); if(!r)return r; m_state=EngineState::Playing; return EngineResult::Success();}
EngineResult MediaEngine::Pause(){ Command c{}; c.Type=CommandType::Pause; auto r=QueueCommand(c); if(!r)return r; m_state=EngineState::Paused; return EngineResult::Success();}
EngineResult MediaEngine::Stop(){ Command c{}; c.Type=CommandType::Stop; auto r=QueueCommand(c); if(!r)return r; m_state=EngineState::Stopped; return EngineResult::Success();}
EngineResult MediaEngine::Seek(Milliseconds pos){ Command c{}; c.Type=CommandType::Seek; c.Position=pos; auto r=QueueCommand(c); if(!r)return r; m_state=EngineState::Seeking; return EngineResult::Success();}
EngineState MediaEngine::GetState() const noexcept{return m_state;}
bool MediaEngine::IsInitialized() const noexcept{return m_state!=EngineState::Uninitialized&&m_state!=EngineState::Shutdown;}
bool MediaEngine::IsPlaying() const noexcept{return m_state==EngineState::Playing;}
bool MediaEngine::IsPaused() const noexcept{return m_state==EngineState::Paused;}
}
