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

class WorkStealingQueue {
  public:
    using DataType = FunctionWrapper;

    WorkStealingQueue()                                    = default;
    WorkStealingQueue(const WorkStealingQueue&)            = delete;
    WorkStealingQueue& operator=(const WorkStealingQueue&) = delete;

    void push(DataType data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cout << "[WorkStealingQueue::push]" << std::endl;
        m_the_queue.push_back(std::move(data));
    }

    bool try_pop(DataType& res) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_the_queue.empty()) {
            return false;
        }
        res = std::move(m_the_queue.front());  // front
        m_the_queue.pop_front();
        return true;
    }

    bool try_steal(DataType& res) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_the_queue.empty()) {
            return false;
        }
        res = std::move(m_the_queue.back());  // back
        m_the_queue.pop_back();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_the_queue.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_the_queue.size();
    }

  private:
    std::deque<DataType> m_the_queue;
    mutable std::mutex m_mutex;
};

class ThreadPool {
  public:
    using TaskType = FunctionWrapper;

    ThreadPool() : m_done(false) {
        const unsigned thrad_count = std::thread::hardware_concurrency();
        std::cout << "thread_count: " << thrad_count << std::endl;
        try {
            for (unsigned i = 0; i < thrad_count; ++i) {
                m_local_work_queues.emplace_back(new WorkStealingQueue);
            }
            for (unsigned i = 0; i < thrad_count; ++i) {
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

        for (size_t i = 0; i < m_local_work_queues.size(); ++i) {
            std::cout << "[ThreadPool submit] " << i << " local_work_queues size: " << m_local_work_queues[i]->size()
                      << std::endl;
            if (m_local_work_queues[i]->size() < 5) {
                std::cout << "thread " << std::this_thread::get_id() << " push task to local queue" << std::endl;
                m_local_work_queues[i]->push(std::move(task));
                return res;
            }
        }

        std::cout << "thread " << std::this_thread::get_id() << " push task to pool queue" << std::endl;
        m_pool_work_queue.push(std::move(task));
        return res;
    }

  private:
    void worker_thread(unsigned index) {
        m_queue_index    = index;
        local_work_queue = m_local_work_queues[index].get();
        while (!m_done) {
            TaskType task;
            if (pop_task_from_local_queue(task) || pop_task_from_pool_queue(task) ||
                pop_task_from_other_threads(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

    bool pop_task_from_local_queue(TaskType& task) {
        std::cout << "[ThreadPool pop_task_from_local_queue]" << std::endl;
        if (local_work_queue) {
            return local_work_queue->try_pop(task);
        }
        return false;
    }

    bool pop_task_from_pool_queue(TaskType& task) {
        std::cout << "[ThreadPool pop_task_from_pool_queue]" << std::endl;
        return m_pool_work_queue.try_pop(task);
    }

    bool pop_task_from_other_threads(TaskType& task) {
        for (unsigned i = 0; i < m_local_work_queues.size(); ++i) {
            const unsigned index = (m_queue_index + i + 1) % m_local_work_queues.size();
            WorkStealingQueue* q = m_local_work_queues[index].get();
            std::cout << "[ThreadPool pop_task_from_other_threads] index: " << index << " q: " << q << std::endl;
            if (q->try_steal(task)) {
                std::cout << "thread " << m_queue_index << " steal task from thread " << index << std::endl;
                return true;
            }
        }
        return false;
    }

  private:
    std::atomic_bool m_done;
    ThreadsafeQueue<TaskType> m_pool_work_queue;
    std::vector<std::unique_ptr<WorkStealingQueue>> m_local_work_queues;
    std::vector<std::thread> m_threads;
    static thread_local WorkStealingQueue* local_work_queue;
    static thread_local unsigned m_queue_index;
};

thread_local WorkStealingQueue* ThreadPool::local_work_queue = nullptr;
thread_local unsigned ThreadPool::m_queue_index              = 0;

}  // namespace ccy
