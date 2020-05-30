#pragma once

#include <memory>
#include <functional>
#include <future>

template <typename Data>
class VSChunkUpdate
{
public:
    static std::shared_ptr<VSChunkUpdate<Data>> create(
        std::function<
            void(const Data&, const std::atomic<bool>&, std::atomic<bool>&, std::size_t chunkIndex)>
            updateFunction,
        const Data& data,
        std::size_t chunkIndex)
    {
        const auto chunkUpdate = std::shared_ptr<VSChunkUpdate>(new VSChunkUpdate);
        chunkUpdate->worker = std::thread(
            updateFunction,
            data,
            std::ref(chunkUpdate->bShouldCancel),
            std::ref(chunkUpdate->bIsReady),
            chunkIndex);

        return chunkUpdate;
    };

    void cancel()
    {
        bShouldCancel = true;
        worker.join();
    };

    bool isReady()
    {
        return bIsReady;
    };

    void finish()
    {
        return worker.join();
    };

private:
    VSChunkUpdate() = default;

    std::atomic<bool> bShouldCancel = false;
    std::atomic<bool> bIsReady = false;
    std::thread worker;
};