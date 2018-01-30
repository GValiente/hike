#include <catch.hpp>
#include "hike_thread_pool.h"

TEST_CASE("Empty ThreadPool test")
{
    struct Task
    {
        void operator()()
        {
        }
    };

    hike::ThreadPool<Task> threadPool;
    threadPool.join();
}

TEST_CASE("ThreadPool test")
{
    struct Task
    {
        int& param;

        void operator()()
        {
            param = 1;
        }
    };

    std::vector<int> params(10000, 0);
    hike::ThreadPool<Task> threadPool;

    for(int& param : params)
    {
        REQUIRE(param == 0);
        threadPool.add(Task{param});
    }

    threadPool.join();

    for(int param : params)
    {
        REQUIRE(param == 1);
    }
}
