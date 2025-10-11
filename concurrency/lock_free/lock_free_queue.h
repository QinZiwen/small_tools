#pragma once

#include <atomic>
#include <iostream>
#include <memory>

namespace ccy {

template <typename T>
class LockFreeQueue {
  private:
    struct Node {
        std::shared_ptr<T> data;
        Node* next;

        Node() : data(nullptr), next(nullptr) {};
    };

    std::atomic<Node*> m_head;
    std::atomic<Node*> m_tail;

    Node* pop_head() {
        Node* old_head = m_head.load();
        if (old_head == m_tail.load()) {
            return nullptr;
        }

        m_head.store(old_head->next);
        return old_head;
    }

  public:
    LockFreeQueue() : m_head(new Node), m_tail(m_head.load()) {}
    LockFreeQueue(const LockFreeQueue*)            = delete;
    LockFreeQueue& operator=(const LockFreeQueue*) = delete;
    ~LockFreeQueue() {
        while (const Node* old_head = m_head.load()) {
            m_head.store(old_head->next);
            delete old_head;
        }
    }

    void push(T new_value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(new_value));
        Node* p        = new Node;
        Node* old_tail = m_tail.load();
        old_tail->data.swap(new_data);
        old_tail = p;
        m_tail.store(p);
    }

    std::shared_ptr<T> pop() {
        Node* old_head = pop_head();
        if (!old_head) {
            return nullptr;
        }

        std::shared_ptr<T> res(old_head->data);
        delete old_head;
        return res;
    }
};

}  // namespace ccy