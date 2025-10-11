#pragma once

#include <memory>
#include <mutex>

namespace ccy {

template <typename T>
class ThreadsafeList {
  private:
    struct Node {
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;

        Node() : data(std::make_shared<T>()), next() {}
        Node(const T& value) : data(std::make_shared<T>(value)), next() {}
    };

    Node m_head;

  public:
    ThreadsafeList() {}
    ~ThreadsafeList() {
        remove_if([](const T&) { return true; });
    }

    ThreadsafeList(const ThreadsafeList&)            = delete;
    ThreadsafeList& operator=(const ThreadsafeList&) = delete;

    void push_front(const T& value) {
        std::unique_ptr<Node> new_node(new Node(value));
        std::lock_guard<std::mutex> l(m_head.m);
        new_node->next = std::move(m_head.next);
        m_head.next    = std::move(new_node);
    }

    template <typename Function>
    void for_each(Function f) {
        Node* current = &m_head;
        std::unique_lock<std::mutex> lock(m_head.m);
        while (Node* next = current->next.get()) {
            std::unique_lock<std::mutex> next_lock(next->m);
            lock.unlock();
            f(*next->data);
            current = next;
            lock    = std::move(next_lock);
        }
    }

    template <typename Predicate>
    std::shared_ptr<T> find_first_if(Predicate p) {
        Node* current = &m_head;
        std::unique_lock<std::mutex> lock(m_head.m);
        while (Node* next = current->next.get()) {
            std::unique_lock<std::mutex> next_lock(next->m);
            lock.unlock();
            if (p(*next->data)) {
                return next->data;
            }
            current = next;
            lock    = std::move(next_lock);
        }

        return std::make_shared<T>();
    }

    template <typename Predicate>
    void remove_if(Predicate p) {
        Node* current = &m_head;
        std::unique_lock<std::mutex> lock(m_head.m);
        while (Node* next = current->next.get()) {
            std::unique_lock<std::mutex> next_lock(next->m);
            if (p(*next->data)) {
                std::unique_ptr<Node> removed_node = std::move(current->next);
                current->next                      = std::move(removed_node->next);
                next_lock.unlock();
            } else {
                lock.unlock();
                current = next;
                lock    = std::move(next_lock);
            }
        }
    }
};

}  // namespace ccy