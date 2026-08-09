#include "ThreadManager.h"

namespace velnix::core
{

ThreadManager::ThreadManager() = default;

ThreadManager::~ThreadManager()
{
    Shutdown();
}

WorkerThread* ThreadManager::CreateThread()
{
    auto thread = std::make_unique<WorkerThread>();

    WorkerThread* ptr = thread.get();

    m_threads.emplace_back(std::move(thread));

    return ptr;
}

void ThreadManager::Shutdown()
{
    for (auto& thread : m_threads)
        thread->Stop();

    m_threads.clear();
}

}