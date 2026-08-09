#pragma once

#include <queue>
#include <mutex>
#include <optional>

#include "Command.h"

namespace velnix::core
{

class CommandQueue final
{
public:

    void Push(const Command& command);

    std::optional<Command> Pop();

    bool Empty() const;

    void Clear();

private:

    mutable std::mutex m_mutex;

    std::queue<Command> m_queue;

};

}