#include "CommandQueue.h"

namespace velnix::core
{

void CommandQueue::Push(const Command& command)
{
    std::lock_guard lock(m_mutex);

    m_queue.push(command);
}

std::optional<Command> CommandQueue::Pop()
{
    std::lock_guard lock(m_mutex);

    if(m_queue.empty())
        return std::nullopt;

    Command cmd = m_queue.front();

    m_queue.pop();

    return cmd;
}

bool CommandQueue::Empty() const
{
    std::lock_guard lock(m_mutex);

    return m_queue.empty();
}

void CommandQueue::Clear()
{
    std::lock_guard lock(m_mutex);

    while(!m_queue.empty())
        m_queue.pop();
}

}