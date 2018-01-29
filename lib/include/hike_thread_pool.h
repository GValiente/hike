// Copyright (c) 2018 Gustavo Valiente gustavo.valiente.m@gmail.com
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef HIKE_THREAD_POOL_H
#define HIKE_THREAD_POOL_H

#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include "hike_common.h"

namespace hike
{

/**
 * @brief Basic thread pool.
 *
 * Based on https://stackoverflow.com/questions/26516683/reusing-thread-in-loop-c
 */
template<class Task>
class ThreadPool
{

public:
    /**
     * @brief Class constructor which manages the number of concurrent threads supported by the implementation.
     */
    ThreadPool() :
        ThreadPool(std::thread::hardware_concurrency())
    {
    }

    /**
     * @brief Class constructor.
     * @param threads Number of threads to manage.
     */
    explicit ThreadPool(unsigned int threads) :
        _pendingTasks(0),
        _exit(false)
    {
        HIKE_ASSERT(threads > 0);

        _threads.reserve(threads);

        for(unsigned int index = 0; index < threads; ++index)
        {
            _threads.emplace_back([this]
            {
                while(true)
                {
                    std::unique_lock<std::mutex> lock(_mutex);

                    while(! _exit && _tasks.empty())
                    {
                        _condition.wait(lock);
                    }

                    if(_tasks.empty())
                    {
                        return;
                    }

                    Task task = std::move(_tasks[0]);
                    _tasks.pop_front();

                    lock.unlock();
                    task();

                    _mutex.lock();
                    --_pendingTasks;
                    _mutex.unlock();
                }
            });
        }
    }

    /**
     * @brief Class destructor.
     */
    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(_mutex);

            _exit = true;
            _condition.notify_all();
        }

        for(auto& thread : _threads)
        {
            thread.join();
        }
    }

    /**
     * @brief Add a task which must be completed by a managed thread.
     */
    template<class TaskType>
    void add(TaskType&& task)
    {
        std::unique_lock<std::mutex> lock(_mutex);

        _tasks.emplace_back(std::forward<TaskType>(task));
        ++_pendingTasks;
        _condition.notify_one();
    }

    /**
     * @brief Wait for all pending tasks to be completed.
     */
    void join()
    {
        while(true)
        {
            int pendingTasks;

            {
                std::unique_lock<std::mutex> lock(_mutex);

                pendingTasks = _pendingTasks;
            }

            if(pendingTasks)
            {
                std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            }
            else
            {
                return;
            }
        }
    }

protected:
    ///@cond INTERNAL

    std::mutex _mutex;
    std::condition_variable _condition;
    int _pendingTasks;
    bool _exit;
    std::deque<Task> _tasks;
    std::vector<std::thread> _threads;

    ///@endcond
};

}

#endif
