#pragma once
#include "thread_safe_queue.hpp"

#include <concepts>
#include <functional>
#include <iostream>
#include <semaphore>
#include <stdint.h>
#include <thread>
#include <vector>

auto log_msg = [](size_t thread_id, auto msg) {
    std::clog << "[thread " << thread_id << "]: " << msg << std::endl;
};

template <typename Task = std::function<void()>>
    requires std::invocable<Task>
class thread_pool {
public:
    thread_pool(const uint8_t num_threads = std::thread::hardware_concurrency())
        : num_threads(num_threads)
        , queues(num_threads) {
        for (size_t i = 0; i < num_threads; i++) {
            semaphores.emplace_back(std::make_unique<std::binary_semaphore>(0));
        }
        for (size_t i = 0; i < num_threads; i++) {
            threads.emplace_back(&thread_pool::worker_thread, this, i);
        }
    }
    ~thread_pool() {
        
        stopping = true;
        for (int i = 0; i < num_threads; i++) {
            semaphores[i]->release(); // wake up any waiting threads
            log_msg(i, "joining");
            threads[i].join();
            log_msg(i, "finished joining");
        }
        
    }
    void enqueue_task(Task t) {
        if (!stopping) {
            size_t idx = std::rand() % num_threads;
            queues[idx].push_back(std::move(t));
            log_msg(idx, "enqueueing task");
            semaphores[idx]->release();
        }
    }

private:
    const uint8_t num_threads;
    std::vector<thread_safe_queue<Task>> queues;
    std::vector<std::thread> threads;
    std::vector<std::unique_ptr<std::binary_semaphore>> semaphores;
    std::atomic<bool> stopping{false};
    void worker_thread(const size_t thread_id) {
        log_msg(thread_id, "thread created");
        while (true) {
            Task curr_task;
            while (auto task = queues[thread_id].pop_front()) { // execute all tasks in queue
                curr_task = std::move(task.value());
                log_msg(thread_id, "executing task");
                curr_task();
                log_msg(thread_id, "finished executing task");
            }
            if (steal_task(curr_task, thread_id)) { // attempt to steal task
                log_msg(thread_id, "executing stolen task");
                curr_task();
                log_msg(thread_id, "finished executing stolen task");
            } else if (stopping) {
                break;
            } else if (queues[thread_id].empty()) {
                log_msg(thread_id, "sleeping");
                semaphores[thread_id]->acquire();
                log_msg(thread_id, "woke up");
            }
        }
    }
    bool steal_task(Task &stolen_task, size_t thread_id) {
        for (size_t i = 1; i < num_threads; i++) {
            auto task = queues[(thread_id + i) % num_threads].pop_back();
            if (task) {
                log_msg(thread_id, "stole task from thread" + std::to_string(thread_id + i % num_threads));
                stolen_task = std::move(task.value());
                return true;
            }
        }
        return false;
    }
};
