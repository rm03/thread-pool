#include "thread_pool.hpp"
#include <iostream>

auto log_msg = [](size_t thread_id, auto msg) {
    std::clog << "[thread " << thread_id << "]: " << msg << std::endl;
};

thread_pool::thread_pool(const uint8_t num_threads)
    : num_threads(num_threads)
    , queues(num_threads) {
    for (size_t i = 0; i < num_threads; i++) {
        threads.emplace_back(&thread_pool::worker_thread, this, i);
        semaphores.emplace_back(std::make_unique<std::binary_semaphore>(0));
    }
}

thread_pool::~thread_pool() {
    stopping = true;
    for (int i = 0; i < num_threads; i++) {
        semaphores[i]->release(); // wake up any waiting threads
        log_msg(i, "joining");
        threads[i].join();
    }
}

void thread_pool::enqueue_task(task t) {
    if (!stopping) {
        size_t idx = std::rand() % num_threads;
        queues[idx].push_back(std::move(t));
        log_msg(idx, "enqueueing task");
        semaphores[idx]->release();
    }
}

void thread_pool::worker_thread(const size_t thread_id) {
    while (true) {
        task curr_task;
        while (auto task = queues[thread_id].pop_front()) { // execute all tasks in queue
            curr_task = std::move(task.value());
            log_msg(thread_id, "executing task");
            curr_task();
            log_msg(thread_id, "finished executing task");
        }
        if (steal_task(curr_task, thread_id)) { // attempt to steal task
            log_msg(thread_id, "executing STOLEN task");
            curr_task();
            log_msg(thread_id, "finished executing STOLEN task");
        } else if (stopping) {
            break;
        } else if (queues[thread_id].empty()) {
            log_msg(thread_id, "sleeping");
            semaphores[thread_id]->acquire();
            log_msg(thread_id, "woke up");
        }
    }
}

bool thread_pool::steal_task(task &stolen_task, size_t thread_id) {
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
