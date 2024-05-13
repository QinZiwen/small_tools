#pragma once

#include <atomic>
#include <functional>
#include <vector>
#include <thread>
#include <iostream>

#include "threadsafe_queue.h"

namespace ccy {

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
    void submit(FunctionType f) {
        m_work_queue.push(std::function<void()>(f));
    }

private:
    void worker_thread() {
        while (!m_done) {
            std::function<void()> task;
            if (m_work_queue.try_pop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

private:
    std::atomic_bool m_done;
    ThreadsafeQueue<std::function<void()>> m_work_queue;
    std::vector<std::thread> m_threads;
};

}  // namespace ccy
