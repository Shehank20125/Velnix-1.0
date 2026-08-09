#include "EventQueue.h"

namespace velnix::core
{

void EventQueue::Push(const Event& event)
{
    m_queue.Push(event);
}

Event EventQueue::Wait()
{
    return m_queue.WaitAndPop();
}

bool EventQueue::Empty() const
{
    return m_queue.Empty();
}

void EventQueue::Shutdown()
{
    m_queue.Shutdown();
}

}