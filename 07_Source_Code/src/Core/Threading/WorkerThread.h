#pragma once

/**
 * @file WorkerThread.h
 */

#include <atomic>
#include <functional>
#include <string>
#include <thread>

namespace velnix::core
{

class WorkerThread final
{
public:

    using Task = std::function<void()>;

    WorkerThread();

    ~WorkerThread();

    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator=(const WorkerThread&) = delete;

    bool Start(Task task);

    void Stop();

    bool Running() const noexcept;

private:

    std::thread m_thread;

    std::atomic<bool> m_running{ false };

};

}