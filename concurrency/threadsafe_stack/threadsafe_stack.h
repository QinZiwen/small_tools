#pragma once

#include <exception>
#include <memory>
#include <mutex>
#include <stack>

namespace ccy {

struct EmptyStack : public std::exception {
    const char* what() const noexcept override {
        return "Empty stack";
    }
};

template <typename T>
class ThreadsafeStack {
  public:
    ThreadsafeStack() = default;
    ThreadsafeStack(const ThreadsafeStack& other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_data = other.m_data;
    }
    ThreadsafeStack& operator=(const ThreadsafeStack& other) = delete;

    void push(T val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data.push(val);
    }

    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data.empty()) {
            throw EmptyStack();
        }
        std::shared_ptr<T> res(std::make_shared<T>(m_data.top()));
        m_data.pop();
        return res;
    }

    void pop(T& val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data.empty()) {
            throw EmptyStack();
        }
        val = m_data.top();
        m_data.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data.empty();
    }

  private:
    std::stack<T> m_data;
    mutable std::mutex m_mutex;
};

}  // namespace ccy