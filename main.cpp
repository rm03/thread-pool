#include "thread_pool.hpp"
#include <iostream>
#include <chrono>
#include <thread>

void simple_task(int task_id) {
    int random = 1 + (rand() % 10); 
    std::cout << "Task " << task_id << " is running for " << random << " seconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(random)); 
}

int main() {
    const uint8_t num_threads = 4; 
    thread_pool pool(num_threads);

    std::vector<task> tasks;
    for (int i = 0; i < 10; i++) {
        tasks.emplace_back(std::bind(simple_task, i)); 
    }

    pool.load_tasks(tasks); 
    pool.start(); 
}
