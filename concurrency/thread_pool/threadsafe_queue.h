#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace ccy {
    
template<typename T>
class ThreadsafeQueue {
public:
    ThreadsafeQueue() = default;
    ThreadsafeQueue(const ThreadsafeQueue& other) {
        std::lock_guard<std::mutex> lock1(other.m_mutex);
        std::lock_guard<std::mutex> lock2(m_mutex);
        m_data_queue = other.m_data_queue;
    }

    ThreadsafeQueue& operator=(const ThreadsafeQueue& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lock1(other.m_mutex);
            std::lock_guard<std::mutex> lock2(m_mutex);
            m_data_queue = other.m_data_queue;
        }
        return *this;
    }

    void push(T value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data_queue.push(value);
        m_data_cond.notify_one();
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data_queue.empty()) {
            return false;
        }
        value = m_data_queue.front();
        m_data_queue.pop();
        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data_queue.empty()) {
            return nullptr;
        }
        std::shared_ptr<T> res(std::make_shared<T>(m_data_queue.front()));
        m_data_queue.pop();
        return res;
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_data_cond.wait(lock, [this]() { return !m_data_queue.empty(); });
        value = m_data_queue.front();
        m_data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_data_cond.wait(lock, [this]() { return !m_data_queue.empty(); });
        std::shared_ptr<T> res(std::make_shared<T>(m_data_queue.front()));
        m_data_queue.pop();
        return res;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data_queue.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data_queue.size();
    }

private:
    mutable std::mutex m_mutex;
    std::queue<T> m_data_queue;
    std::condition_variable m_data_cond;
};

}  // namespace ccy
