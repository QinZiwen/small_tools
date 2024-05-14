#pragma once

#include <atomic>
#include <functional>
#include <vector>
#include <thread>
#include <iostream>
#include <future>

#include "threadsafe_queue.h"

namespace ccy {

class FunctionWrapper {
public:
    FunctionWrapper() = default;
    template<typename FunctionType>
    FunctionWrapper(FunctionType&& f) : m_imple(new Imple<FunctionType>(std::move(f))) {}
    FunctionWrapper(FunctionWrapper&& other) : m_imple(std::move(other.m_imple)) {}
    FunctionWrapper(const FunctionWrapper&) = delete;
    FunctionWrapper(FunctionWrapper&) = delete;
    FunctionWrapper& operator=(const FunctionWrapper&) = delete;
    FunctionWrapper& operator=(FunctionWrapper&& other) {
        if (this != &other) {
            m_imple = std::move(other.m_imple);
        }
        return *this;
    }

    void operator()() { m_imple->call(); }

private:
    struct ImpleBase {
        virtual void call() = 0;
        virtual ~ImpleBase() = default;
    };

    template<typename FunctionType>
    struct Imple : ImpleBase {
        Imple(FunctionType&& f) : m_function(std::move(f)) {}
        void call() { m_function(); }

        FunctionType m_function;
    };

    std::unique_ptr<ImpleBase> m_imple;
};

class ThreadPool {
public:
    ThreadPool() : m_done(false) {
        const unsigned thrad_count = std::thread::hardware_concurrency();
        std::cout << "thread_count: " << thrad_count << std::endl;
        try {
            for (unsigned i = 0; i < thrad_count; ++i) {
                m_threads.emplace_back(&ThreadPool::worker_thread, this);
            }
        } catch (...) {
            m_done = true;
            throw;
        }
    }

    ~ThreadPool() {
        m_done = true;
        for (auto& t : m_threads) {
            std::cout << t.get_id() << " join ..." << std::endl;
            t.join();
        }
    }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f) {
        using result_type = typename std::result_of<FunctionType()>::type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        m_work_queue.push(std::move(task));
        return res;
    }

private:
    void worker_thread() {
        while (!m_done) {
            FunctionWrapper task;
            if (m_work_queue.try_pop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

private:
    std::atomic_bool m_done;
    ThreadsafeQueue<FunctionWrapper> m_work_queue;
    std::vector<std::thread> m_threads;
};

}  // namespace ccy
