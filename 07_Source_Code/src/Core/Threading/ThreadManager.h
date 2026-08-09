#pragma once

#include <memory>
#include <vector>

#include "WorkerThread.h"

namespace velnix::core
{

class ThreadManager final
{
public:

    ThreadManager();

    ~ThreadManager();

    WorkerThread* CreateThread();

    void Shutdown();

private:

    std::vector<std::unique_ptr<WorkerThread>> m_threads;

};

}