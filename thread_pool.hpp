#pragma once

#include "ts_queue.hpp"
#include <vector> 

using task = std::function<void()>; 

class thread_pool {
    public:
        thread_pool(const uint8_t);
        ~thread_pool();
        void load_tasks(std::vector<task>); 
        void start(); 
    private: 
        std::vector<ts_queue<task>> queues; 
        std::vector<std::thread> threads;
        const uint8_t num_threads; 
}

