# Project Proposal

Our idea is to create a work-stealing thread pool utilizing modern C++ features for efficiency and simplicity.  

![Task system diagram](diagram.png)

As shown above (taken from [Sean Parent's talk](https://www.youtube.com/watch?v=zULU6Hhp42w)), each thread will have its own task queue. When the pool receives a task, it will assign it to a queue in round-robin fashion. Each thread will try and execute tasks from its respective queue; after executing all tasks, it will attempt to steal tasks from the back of other threads' queues. If there are no tasks available to steal, the thread will go to sleep until it receives a task in its queue.

In terms of project structure, we will have two files: one for a thread-safe queue (with a deque as the underlying data structure) and one for the thread pool. We anticipate that the former will take 2-4 hours to complete and the latter will take 4-6 hours. We will work together closely on both of these and potentially split up the work by function. 

For the core implementation, we will likely be using several of C++'s concurrency utilities, concepts, templating, move semantics, and lambdas. We will not be using any external libraries such as Boost; for benchmarking, we may use [Google's benchmark library](https://github.com/google/benchmark). If time permits, we will also add unit tests using [GoogleTest](https://github.com/google/googletest).
