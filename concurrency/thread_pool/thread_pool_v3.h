#pragma once

#include "threadsafe_queue.h"

#include <atomic>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

namespace ccy {

class FunctionWrapper {
  public:
    FunctionWrapper() = default;
    template <typename FunctionType>
    FunctionWrapper(FunctionType&& f) : m_imple(new Imple<FunctionType>(std::move(f))) {}
    FunctionWrapper(FunctionWrapper&& other) : m_imple(std::move(other.m_imple)) {}
    FunctionWrapper(const FunctionWrapper&)            = delete;
    FunctionWrapper(FunctionWrapper&)                  = delete;
    FunctionWrapper& operator=(const FunctionWrapper&) = delete;
    FunctionWrapper& operator=(FunctionWrapper&& other) {
        if (this != &other) {
            m_imple = std::move(other.m_imple);
        }
        return *this;
    }

    void operator()() {
        m_imple->call();
    }

  private:
    struct ImpleBase {
        virtual void call()  = 0;
        virtual ~ImpleBase() = default;
    };

    template <typename FunctionType>
    struct Imple : ImpleBase {
        Imple(FunctionType&& f) : m_function(std::move(f)) {}
        void call() {
            m_function();
        }

        FunctionType m_function;
    };

    std::unique_ptr<ImpleBase> m_imple;
};

class ThreadPool {
  public:
    using QueueType = ThreadsafeQueue<FunctionWrapper>;

    ThreadPool() : m_done(false) {
        const unsigned thrad_count = std::thread::hardware_concurrency();
        std::cout << "thread_count: " << thrad_count << std::endl;
        try {
            for (unsigned i = 0; i < thrad_count; ++i) {
                m_local_work_queues.emplace_back(new QueueType);
                m_threads.emplace_back(&ThreadPool::worker_thread, this, i);
            }
        } catch (...) {
            m_done = true;
            throw;
        }
    }

    ~ThreadPool() {
        m_done = true;
        std::cout << "m_pool_work_queue size: " << m_pool_work_queue.size() << std::endl;
        for (const auto& q : m_local_work_queues) {
            std::cout << "m_local_work_queue size: " << q->size() << std::endl;
        }
        for (auto& t : m_threads) {
            std::cout << t.get_id() << " join ..." << std::endl;
            t.join();
        }
    }

    template <typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f) {
        using result_type = typename std::result_of<FunctionType()>::type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());

        for (auto& q : m_local_work_queues) {
            if (q->size() < 5) {
                std::cout << "thread " << std::this_thread::get_id() << " push task to local queue" << std::endl;
                q->push(std::move(task));
                return res;
            }
        }

        std::cout << "thread " << std::this_thread::get_id() << " push task to pool queue" << std::endl;
        m_pool_work_queue.push(std::move(task));
        return res;
    }

  private:
    void worker_thread(size_t index) {
        QueueType* local_work_queues = nullptr;
        if (!m_local_work_queues.empty()) {
            local_work_queues = m_local_work_queues[index].get();
        }
        std::cout << "index: " << index << ", thread " << std::this_thread::get_id()
                  << " is working, m_local_work_queue: " << local_work_queues << std::endl;

        while (!m_done) {
            FunctionWrapper task;
            if (local_work_queues && !local_work_queues->empty() && local_work_queues->try_pop(task)) {
                std::cout << "thread " << std::this_thread::get_id() << " get task from local queue" << std::endl;
                task();
            } else if (m_pool_work_queue.try_pop(task)) {
                std::cout << "thread " << std::this_thread::get_id() << " get task from pool queue" << std::endl;
                task();
                if (local_work_queues && local_work_queues->empty()) {
                    for (int i = 0; i < 24; ++i) {
                        if (m_pool_work_queue.try_pop(task)) {
                            local_work_queues->push(std::move(task));
                        } else {
                            continue;
                        }
                    }
                }
            } else {
                // std::cout << "thread " << std::this_thread::get_id() << " is idle" << std::endl;
                std::this_thread::yield();
            }
        }
    }

  private:
    std::atomic_bool m_done;
    std::vector<std::thread> m_threads;
    QueueType m_pool_work_queue;
    std::vector<std::unique_ptr<QueueType>> m_local_work_queues;
};

}  // namespace ccy
