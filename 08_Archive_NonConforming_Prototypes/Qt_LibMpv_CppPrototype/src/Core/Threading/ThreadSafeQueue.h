#pragma once

/**
 * @file ThreadSafeQueue.h
 * @brief Generic thread-safe FIFO queue.
 */

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace velnix::core
{

template<typename T>
class ThreadSafeQueue final
{
public:

    ThreadSafeQueue() = default;

    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    void Push(const T& item)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(item);
        }

        m_condition.notify_one();
    }

    void Push(T&& item)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::move(item));
        }

        m_condition.notify_one();
    }

    std::optional<T> TryPop()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_queue.empty())
            return std::nullopt;

        T value = std::move(m_queue.front());

        m_queue.pop();

        return value;
    }

    T WaitAndPop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_condition.wait(lock,
            [this]
            {
                return !m_queue.empty() || m_shutdown;
            });

        if (m_shutdown)
            return T{};

        T value = std::move(m_queue.front());

        m_queue.pop();

        return value;
    }

    bool Empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        return m_queue.empty();
    }

    std::size_t Size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        return m_queue.size();
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        while (!m_queue.empty())
            m_queue.pop();
    }

    void Shutdown()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            m_shutdown = true;
        }

        m_condition.notify_all();
    }

private:

    mutable std::mutex m_mutex;

    std::condition_variable m_condition;

    std::queue<T> m_queue;

    bool m_shutdown = false;
};

}