#include "WorkerThread.h"

namespace velnix::core
{

WorkerThread::WorkerThread() = default;

WorkerThread::~WorkerThread()
{
    Stop();
}

bool WorkerThread::Start(Task task)
{
    if (m_running)
        return false;

    m_running = true;

    m_thread = std::thread(
        [this, task]()
        {
            task();

            m_running = false;
        });

    return true;
}

void WorkerThread::Stop()
{
    if (m_thread.joinable())
        m_thread.join();

    m_running = false;
}

bool WorkerThread::Running() const noexcept
{
    return m_running;
}

}