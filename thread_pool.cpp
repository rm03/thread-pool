#include "thread_pool.h"

thread_pool::thread_pool(const uint8_t num_thread) : num_threads(num_thread) {
    queues (num_threads); // queue for each thread
    threads.reserve(num_threads); 
}

void thread_pool::load_tasks(std::vector<task> tasks) {
    for (size_t i = 0; i < tasks.size(); i++) {
        queues[i % num_threads].push_back(std::move(tasks[i]));
    }
}

void thread_pool::start() {

} 
