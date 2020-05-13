#include "core/vs_barrier.h"

VSBarrier::VSBarrier(std::size_t iCount)
    : threshold(iCount)
    , count(iCount)
    , generation(0)
{
}

void VSBarrier::Wait()
{
    std::unique_lock<std::mutex> lLock{mutex};
    auto lGen = generation;
    if (!--count)
    {
        generation++;
        count = threshold;
        cond.notify_all();
    }
    else
    {
        cond.wait(lLock, [this, lGen] { return lGen != generation; });
    }
}