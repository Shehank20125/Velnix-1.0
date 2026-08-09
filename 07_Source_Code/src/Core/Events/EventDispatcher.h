#pragma once

/**
 * @file EventDispatcher.h
 */

#include <memory>
#include <mutex>
#include <vector>

#include "EventListener.h"

namespace velnix::core
{

class EventDispatcher final
{
public:

    EventDispatcher() = default;

    ~EventDispatcher() = default;

    void AddListener(EventListener* listener);

    void RemoveListener(EventListener* listener);

    void Dispatch(const Event& event);

private:

    std::mutex m_mutex;

    std::vector<EventListener*> m_listeners;
};

}