#include <deque>
#include <mutex>
#include <shared_mutex>
#include <optional>

template <typename T>
class ts_queue {
public:
    void push_front(T&& elt) {
        std::scoped_lock lock(mutex);
        queue.push_front(std::forward<T>(elt));
    }

    void push_back(T&& elt) {
        std::scoped_lock lock(mutex);
        queue.push_back(std::forward<T>(elt));
    }

    std::optional<T> pop_front() {
        std::scoped_lock lock(mutex);
        if (queue.empty()) {
            return std::nullopt; 
        }
        T elt = std::move(queue.front());
        queue.pop_front(); 
        return elt;
    }

    std::optional<T> pop_back() {
        std::scoped_lock lock(mutex);
        if (queue.empty()) {
            return std::nullopt; 
        }
        T elt = std::move(queue.back()); 
        queue.pop_back();
        return elt;
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
