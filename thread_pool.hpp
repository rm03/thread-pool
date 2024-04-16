#pragma once

#include "ts_queue.hpp"
#include <vector> 

class thread_pool {
    public:
        thread_pool(const uint8_t);
        ~thread_pool(); 
    private: 
        std::vector<ts_queue> queues; 
        std::vector<std::thread> threads;
        const uint8_t num_threads; 
}

