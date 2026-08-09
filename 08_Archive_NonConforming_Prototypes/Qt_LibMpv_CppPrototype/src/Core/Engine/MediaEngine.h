#pragma once
#include "../Common/Types.h"
#include "EngineResult.h"
#include "EngineState.h"
namespace velnix::core{
class SessionManager; class ThreadManager; class EventDispatcher; class CommandQueue; struct Command;
class MediaEngine final{
public:
 MediaEngine(); ~MediaEngine();
 MediaEngine(const MediaEngine&)=delete; MediaEngine& operator=(const MediaEngine&)=delete;
 EngineResult Initialize(); EngineResult Shutdown();
 EngineResult Open(const Path&); EngineResult Close(); EngineResult Play(); EngineResult Pause(); EngineResult Stop(); EngineResult Seek(Milliseconds);
 EngineState GetState() const noexcept; bool IsInitialized() const noexcept; bool IsPlaying() const noexcept; bool IsPaused() const noexcept;
private:
 EngineResult QueueCommand(const Command&);
 EngineState m_state{EngineState::Uninitialized};
 UniquePtr<SessionManager> m_session; UniquePtr<ThreadManager> m_threads; UniquePtr<EventDispatcher> m_events; UniquePtr<CommandQueue> m_commands;
};}
