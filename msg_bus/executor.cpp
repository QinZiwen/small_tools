#include "executor.h"

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class ThreadPool {
  public:
    // 构造函数，初始化线程池
    ThreadPool(size_t threads);

    // 析构函数，清理线程池
    ~ThreadPool();

    // 禁止拷贝构造和赋值
    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // 向线程池添加任务
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

  private:
    // 工作线程集合
    std::vector<std::thread> workers;

    // 任务队列
    std::queue<std::function<void()>> tasks;

    // 同步原语
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// 构造函数实现
inline ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    // 加锁等待任务
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

                    // 如果线程池停止且没有任务，退出
                    if (this->stop && this->tasks.empty())
                        return;

                    // 取出任务
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }

                // 执行任务
                task();
            }
        });
    }
}

// 析构函数实现
inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }

    // 通知所有线程停止
    condition.notify_all();

    // 等待所有线程完成
    for (std::thread& worker : workers) {
        worker.join();
    }
}

// 添加任务到线程池
template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    // 确定任务返回类型
    using return_type = typename std::result_of<F(Args...)>::type;

    // 包装任务为可调用对象
    auto task =
        std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    // 获取任务的future
    std::future<return_type> res = task->get_future();

    {
        // 加锁添加任务
        std::unique_lock<std::mutex> lock(queue_mutex);

        // 禁止在线程池停止后添加新任务
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        // 将任务添加到队列
        tasks.emplace([task]() { (*task)(); });
    }

    // 通知一个等待的线程有新任务
    condition.notify_one();

    return res;
}

struct SyncExecutor : Executor {
    void dispatch(std::function<void()> task) override {
        task();
    }
} _syncExecutor;

struct AsyncExecutor : Executor {
    enum { PARALLEL = 3 };
    ThreadPool threadPool;

    AsyncExecutor() : threadPool(PARALLEL) {}

    void dispatch(std::function<void()> task) override {
        threadPool.enqueue(task);
    }
} _asyncExecutor;

using Map = std::unordered_map<Executor::Id, Executor*>;
struct ExecutorMap : public std::mutex, public Map {
    ExecutorMap() {
        (*(Map*)this)[Executor::Sync]  = &_syncExecutor;
        (*(Map*)this)[Executor::Async] = &_asyncExecutor;
    }
} _executorMap;

void Executor::mount(Id id) {
    std::lock_guard<ExecutorMap> lock(_executorMap);
    _executorMap[id] = this;
}

void Executor::demount(Id id) {
    std::lock_guard<ExecutorMap> lock(_executorMap);
    // Executor* old = _executorMap[id];
    _executorMap.erase(id);
}

Executor* Executor::get(Id id) {
    std::lock_guard<ExecutorMap> lock(_executorMap);
    auto executor = _executorMap.find(id);
    return executor == _executorMap.end() ? _executorMap[Executor::Sync] : executor->second;
}