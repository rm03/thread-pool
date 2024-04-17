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
            }
        } else if (steal_task(curr_task, thread_id)) { 
            curr_task(); 
        } else { // should put work stealing logic here
            break;
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

bool thread_pool::steal_task(task& stolen_task, size_t thread_id) {
    for (size_t i = 0; i < num_threads; i++) {
        if (thread_id != i) {
            auto optional_val = queues[i].pop_back(); 
            if (optional_val) {
                std::cout << "Stealing task from thread " << i << " to thread " << thread_id << std::endl;
                stolen_task = std::move(optional_val.value()); 
                return true; 
            }
        }
    }
    return false; 
}
