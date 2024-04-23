#include "thread_pool.hpp"
#include <chrono>
#include <iostream>
#include <thread>

void simple_task(int task_id) {
    int random = 1 + (rand() % 10);
    std::cerr << "Task " << task_id << " is running for " << random << " seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(random));
}

int main() {
    const uint8_t num_threads = 4;
    thread_pool pool(num_threads);

    for (int i = 0; i < 10; i++) {
        pool.enqueue_task(std::bind(simple_task, i));
    }
    std::this_thread::sleep_for(std::chrono::minutes(1));
}
