#pragma once
#include "ts_queue.hpp"

#include <functional>
#include <semaphore>
#include <stdint.h>
#include <thread>
#include <vector>

using task = std::function<void()>;

class thread_pool {
public:
    thread_pool(const uint8_t num_threads = std::thread::hardware_concurrency());
    ~thread_pool();
    void enqueue_task(task);

private:
    const uint8_t num_threads;
    std::vector<ts_queue<task>> queues;
    std::vector<std::thread> threads;
    std::vector<std::unique_ptr<std::binary_semaphore>> semaphores;
    std::atomic<bool> stopping{false};
    void worker_thread(const size_t thread_id);
    bool steal_task(task &stolen_task, size_t thread_id);
};
