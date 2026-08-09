#pragma once

/**
 * @file EventQueue.h
 */

#include "../Threading/ThreadSafeQueue.h"
#include "Event.h"

namespace velnix::core
{

class EventQueue final
{
public:

    void Push(const Event& event);

    Event Wait();

    bool Empty() const;

    void Shutdown();

private:

    ThreadSafeQueue<Event> m_queue;
};

}