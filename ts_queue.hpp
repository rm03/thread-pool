#include <deque>
#include <mutex>
#include <shared_mutex>

template <typename T>
class ts_queue {
public:
    void push_front(T elt) {
        std::scoped_lock lock(mutex);
        queue.push_front(elt);
    }

    void push_back(T elt) {
        std::scoped_lock lock(mutex);
        queue.push_back(elt);
    }

    std::optional<T> pop_front() {
        std::scoped_lock lock(mutex);
        return queue.empty() ? std::nullopt : queue.pop_front();
    }

    std::optional<T> pop_back() {
        std::scoped_lock lock(mutex);
        return queue.empty() ? std::nullopt : queue.pop_back();
    }

    bool empty() {
        std::shared_lock<std::shared_mutex> lock(shared_mutex);
        return queue.empty();
    }

private:
    std::deque<T> queue;
    std::mutex mutex;
    std::shared_mutex shared_mutex;
};
