#pragma once

#include <memory>
#include <functional>
#include <future>

template <typename Result>
class VSChunkUpdate
{
public:
    static std::shared_ptr<VSChunkUpdate<Result>> create(
        std::function<Result(
            const std::atomic<bool>&,
            std::atomic<bool>&,
            std::size_t chunkIndex)> updateFunction,
        std::size_t chunkIndex)
    {
        const auto chunkUpdate = std::shared_ptr<VSChunkUpdate>(new VSChunkUpdate);
        chunkUpdate->result = std::async(
            std::launch::async,
            updateFunction,
            std::ref(chunkUpdate->bShouldCancel),
            std::ref(chunkUpdate->bIsReady),
            chunkIndex);

        return chunkUpdate;
    };

    void cancel()
    {
        bShouldCancel = true;
        result.wait();
    };

    bool isReady()
    {
        return bIsReady;
    };

    Result getResult()
    {
        return result.get();
    };

private:
    VSChunkUpdate() = default;

    std::atomic<bool> bShouldCancel = false;
    std::atomic<bool> bIsReady = false;
    std::future<Result> result;
};