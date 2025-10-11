#pragma once

#include <atomic>
#include <iostream>
#include <memory>

namespace ccy {

template <typename T>
class LockFreeStack {
  private:
    struct Node;
    struct CountedNodePtr {
        int external_count;
        Node* ptr;

        CountedNodePtr() : external_count(0), ptr(nullptr) {
            std::cout << "construct CountedNodePtr, this: " << this << std::endl;
        }
    };

    struct Node {
        std::shared_ptr<T> data;
        std::atomic<int> internal_count;
        CountedNodePtr next;

        Node(const T& d) : data(std::make_shared<T>(d)), internal_count(0), next() {
            std::cout << "construct Node" << std::endl;
        }
    };

    std::atomic<CountedNodePtr> m_head;

    void increaseHeadCount(CountedNodePtr& old_counter) {
        CountedNodePtr new_counter;
        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!m_head.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire,
                                                 std::memory_order_relaxed));
        old_counter.external_count = new_counter.external_count;
    }

  public:
    LockFreeStack() {
        std::cout << "construct LockFreeStack, this: " << this << std::endl;
    }
    ~LockFreeStack() {
        while (pop())
            ;
    }

    void push(const T& data) {
        CountedNodePtr new_node;
        new_node.ptr            = new Node(data);
        new_node.external_count = 1;
        new_node.ptr->next      = m_head.load(std::memory_order_relaxed);
        while (!m_head.compare_exchange_weak(new_node.ptr->next, new_node, std::memory_order_relaxed,
                                             std::memory_order_relaxed))
            ;
    }

    std::shared_ptr<T> pop() {
        CountedNodePtr old_head = m_head.load(std::memory_order_relaxed);
        while (true) {
            increaseHeadCount(old_head);
            Node* ptr = old_head.ptr;
            if (!ptr) {
                return std::shared_ptr<T>();
            }

            std::cout << "pop, compare_exchange_strong, data: " << *(ptr->data) << std::endl;
            if (m_head.compare_exchange_strong(old_head, ptr->next, std::memory_order_relaxed)) {
                std::shared_ptr<T> res;
                res.swap(ptr->data);
                int count_increase = old_head.external_count - 2;
                if (ptr->internal_count.fetch_add(count_increase, std::memory_order_release) == -count_increase) {
                    std::cout << "pop, delete ptr, old_head.external_count: " << old_head.external_count
                              << " count_increase: " << count_increase
                              << " internal_count: " << ptr->internal_count.load() << std::endl;
                    delete ptr;
                }
                std::cout << "pop return res: " << *res << std::endl;
                return res;
            } else if (ptr->internal_count.fetch_add(-1, std::memory_order_relaxed) == 1) {
                std::cout << "pop, -1, internal_count: " << ptr->internal_count.load(std::memory_order_acquire)
                          << std::endl;
                delete ptr;
            } else {
                std::cout << "pop, else" << std::endl;
            }
        }
    }
};

}  // namespace ccy