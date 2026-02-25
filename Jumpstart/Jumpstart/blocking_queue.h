#pragma once
#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T>
class BlockingQueue {
public:

    void push(T v) {
        {
            std::lock_guard<std::mutex> lock(m_);
            q_.push(std::move(v));
        }
        cv_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m_);
        cv_.wait(lock, [&] { return !q_.empty() || stop_; });
        if (stop_ && q_.empty())
            return T{};
        T v = std::move(q_.front());
        q_.pop();
        return v;
    }

    bool try_pop(T& out) {
        std::lock_guard<std::mutex> lock(m_);
        if (q_.empty()) return false;
        out = std::move(q_.front());
        q_.pop();
        return true;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(m_);
            stop_ = true;
        }
        cv_.notify_all();
    }

private:

    std::mutex m_;
    std::condition_variable cv_;
    std::queue<T> q_;
    bool stop_ = false;
};