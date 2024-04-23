#include <deque>
#include <mutex>
#include <optional>

template <typename T>
class thread_safe_queue {
public:
    void push_front(T &&elt) {
        std::scoped_lock lock(mutex);
        queue.push_front(std::forward<T>(elt));
    }

    void push_back(T &&elt) {
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
        std::scoped_lock lock(mutex);
        return queue.empty();
    }

private:
    std::deque<T> queue;
    std::mutex mutex;
};
