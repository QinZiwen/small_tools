#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace ccy {

template <typename T>
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

template <typename T>
class ThreadsafeQueueV2 {
  public:
    ThreadsafeQueueV2() : m_head(new Node), m_tail(m_head.get()) {}

    ThreadsafeQueueV2(const ThreadsafeQueueV2& other)            = delete;
    ThreadsafeQueueV2& operator=(const ThreadsafeQueueV2& other) = delete;

    std::shared_ptr<T> try_pop() {
        if (m_head.get() == m_tail) {
            return nullptr;
        }

        std::lock_guard<std::mutex> lock(m_head_mutex);
        std::shared_ptr<T> res(std::move(m_head->data));
        std::unique_ptr<Node> old_head = std::move(m_head);
        m_head                         = std::move(old_head->next);
        return res;
    }

    void push(T value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(value)));
        std::unique_ptr<Node> p(new Node);

        std::lock_guard<std::mutex> lock(m_tail_mutex);
        m_tail->data = std::move(new_data);
        m_tail->next = std::move(p);
        m_tail       = m_tail->next.get();
    }

  private:
    struct Node {
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;
    };

  private:
    std::unique_ptr<Node> m_head;
    Node* m_tail;
    std::mutex m_head_mutex;
    std::mutex m_tail_mutex;
};

template <typename T>
class ThreadsafeQueueV3 {
  public:
    ThreadsafeQueueV3() : m_head(new Node), m_tail(m_head.get()) {}

    ThreadsafeQueueV3(const ThreadsafeQueueV3& other)            = delete;
    ThreadsafeQueueV3& operator=(const ThreadsafeQueueV3& other) = delete;

    std::shared_ptr<T> try_pop() {
        std::unique_ptr<Node> old_head = pop_head();
        return old_head ? old_head->data : nullptr;
    }

    void push(T value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(value)));
        std::unique_ptr<Node> p(new Node);

        std::lock_guard<std::mutex> lock(m_tail_mutex);
        m_tail->data = std::move(new_data);
        m_tail->next = std::move(p);
        m_tail       = m_tail->next.get();
    }

  private:
    struct Node {
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;
    };

    std::unique_ptr<Node> pop_head() {
        std::lock_guard<std::mutex> lock(m_head_mutex);
        if (m_head.get() == m_tail) {
            return nullptr;
        }
        std::unique_ptr<Node> old_head = std::move(m_head);
        m_head                         = std::move(old_head->next);
        return old_head;
    }

  private:
    std::unique_ptr<Node> m_head;
    Node* m_tail;
    std::mutex m_head_mutex;
    std::mutex m_tail_mutex;
};

template <typename T>
class ThreadsafeQueueV4 {
  public:
    ThreadsafeQueueV4() : m_head(new Node), m_tail(m_head.get()) {}

    ThreadsafeQueueV4(const ThreadsafeQueueV4& other)            = delete;
    ThreadsafeQueueV4& operator=(const ThreadsafeQueueV4& other) = delete;

    std::shared_ptr<T> wait_pop() {
        std::unique_ptr<Node> old_head = pop_head();
        return old_head->data;
    }

    void push(T value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(value)));
        std::unique_ptr<Node> p(new Node);

        std::lock_guard<std::mutex> lock(m_tail_mutex);
        m_tail->data = std::move(new_data);
        m_tail->next = std::move(p);
        m_tail       = m_tail->next.get();
        m_cond.notify_one();
    }

  private:
    struct Node {
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;
    };

    std::unique_ptr<Node> pop_head() {
        std::unique_lock<std::mutex> lock(m_head_mutex);
        m_cond.wait(lock, [this]() { return m_head.get() != m_tail; });
        std::unique_ptr<Node> old_head = std::move(m_head);
        m_head                         = std::move(old_head->next);
        return old_head;
    }

  private:
    std::unique_ptr<Node> m_head;
    Node* m_tail;
    std::mutex m_head_mutex;
    std::mutex m_tail_mutex;
    std::condition_variable m_cond;
};

}  // namespace ccy