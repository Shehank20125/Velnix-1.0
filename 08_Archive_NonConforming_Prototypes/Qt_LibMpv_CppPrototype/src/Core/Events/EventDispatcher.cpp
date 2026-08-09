#include "EventDispatcher.h"

#include <algorithm>

namespace velnix::core
{

void EventDispatcher::AddListener(EventListener* listener)
{
    if (!listener)
        return;

    std::lock_guard<std::mutex> lock(m_mutex);

    m_listeners.push_back(listener);
}

void EventDispatcher::RemoveListener(EventListener* listener)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_listeners.erase(
        std::remove(
            m_listeners.begin(),
            m_listeners.end(),
            listener),
        m_listeners.end());
}

void EventDispatcher::Dispatch(const Event& event)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto* listener : m_listeners)
    {
        if (listener)
        {
            listener->OnEvent(event);
        }
    }
}
}