#pragma once

#include <cstddef>
#include <mutex>

class VSBarrier {
public:
    explicit VSBarrier(std::size_t iCount);

    void Wait();

private:
    std::mutex mutex;
    std::condition_variable cond;
    std::size_t threshold;
    std::size_t count;
    std::size_t generation;
};