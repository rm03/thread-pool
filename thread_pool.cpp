#include "thread_pool.hpp"

thread_pool::thread_pool(const uint8_t num_thread) : num_threads(num_thread), queues(num_thread), stop_flag(false) {
    threads.reserve(num_threads); 
}

thread_pool::~thread_pool() {
    stop(); 
}

void thread_pool::load_tasks(std::vector<task> tasks) {
    for (size_t i = 0; i < tasks.size(); i++) {
        queues[i % num_threads].push_back(std::move(tasks[i]));
    }
}

void thread_pool::start() {
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(&thread_pool::worker_thread, this, i); 
    }
}

void thread_pool::worker_thread(const size_t thread_id) {
    while (true) {
        task curr_task; 
        if (!queues[thread_id].empty()) {
            auto optional_val = queues[thread_id].pop_front(); 
            if (optional_val) {
                curr_task = std::move(optional_val.value()); 
                curr_task(); 
                std::cout << thread_id << std::endl; 
            }
        } else if (all_queues_empty()) { 
            // order should be flipped at some point, so it should try stealing first 
            // and if it's not able to find any tasks, then moves on
            break;
        } else { // should put work stealing logic here
            std::this_thread::yield(); 
        }
    }
}

bool thread_pool::all_queues_empty() {
    for (auto& q : queues) {
        if (!q.empty()) {
            return false; 
        }
    }
    return true; 
}

void thread_pool::stop() {
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join(); 
        }
    }
}
