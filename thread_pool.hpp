#pragma once

#include <optional>
#include <stdint.h>
#include "ts_queue.hpp"
#include <vector>
#include <thread>
#include <iostream>

using task = std::function<void()>; 

class thread_pool {
    public:
        thread_pool(const uint8_t);
        ~thread_pool();
        void load_tasks(std::vector<task>); 
        void start();
        void stop(); 
    private: 
        const uint8_t num_threads;
        std::vector<ts_queue<task>> queues; 
        std::vector<std::thread> threads;
        void worker_thread(const size_t thread_id);
        //bool steal_task(task& stolen_task, size_t thread_id);
        bool all_queues_empty();

        std::atomic<bool> stop_flag; 
};

