#pragma once
#include "thread_safe_queue.hpp"

#include <concepts>
#include <functional>
#include <iostream>
#include <semaphore>
#include <stdint.h>
#include <thread>
#include <vector>
#include <random>
#include <array>
#include <optional>

template <typename Task = std::function<void()>>
    requires std::invocable<Task>
class thread_pool {
public:
    thread_pool(const uint8_t num_threads = std::thread::hardware_concurrency())
        : num_threads(num_threads)
        , queues(num_threads)
        , rng(std::random_device{}()) 
        , dist(0, static_cast<size_t>(num_threads - 1)){
        for (size_t i = 0; i < num_threads; i++) {
            semaphores[i].emplace(0);
        }
        threads.reserve(num_threads);
        for (size_t i = 0; i < num_threads; i++) {
            threads.emplace_back(&thread_pool::worker_thread, this, i);
        }
    }
    ~thread_pool() {

        stopping = true;
        for (int i = 0; i < num_threads; i++) {
            semaphores[i]->release(); // wake up any waiting threads
            threads[i].join();
        }
    }
    inline void enqueue_task(Task t) {
        size_t idx = dist(rng) % num_threads;
        queues[idx].push_back(std::move(t));
        semaphores[idx]->release();
    }

private:
    const uint8_t num_threads;
    std::atomic<bool> stopping{false};
    std::vector<thread_safe_queue<Task>> queues;
    std::vector<std::thread> threads;
    std::array<std::optional<std::binary_semaphore>, 64> semaphores;
    std::mt19937 rng;
    std::uniform_int_distribution<size_t> dist;
    void worker_thread(const size_t thread_id) {
        while (true) {
            Task curr_task;
            while (auto task = queues[thread_id].pop_front()) { // execute all tasks in queue
                curr_task = std::move(task.value());
                curr_task();
            }
            if (steal_task(curr_task, thread_id)) { // attempt to steal task
                curr_task();
            } else if (stopping) {
                break;
            } else if (queues[thread_id].empty()) {
                semaphores[thread_id]->acquire();
            }
        }
    }
    inline bool steal_task(Task &stolen_task, size_t thread_id) {
        for (size_t i = 1; i < num_threads; i++) {
            auto task = queues[(thread_id + i) % num_threads].pop_back();
            if (task) {
                stolen_task = std::move(task.value());
                return true;
            }
        }
        return false;
    }
};
