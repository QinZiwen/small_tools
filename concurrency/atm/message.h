#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <iostream>

namespace ccy {

struct MessageBase {
    virtual ~MessageBase() = default;
};

// 使用了多态和模板，使得消息类型可以自定义，同时又可以将其封装在消息队列中
template <typename Msg>
struct WrappedMessage : public MessageBase {
    Msg contents;

    explicit WrappedMessage(const Msg& msg) : contents(msg) {}
};

class Queue {
public:
    template <typename T>
    void push(const T& msg) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(std::make_shared<WrappedMessage<T>>(msg));
        m_condition.notify_all();
        std::cout << "[Queue::push] message: " << typeid(T).name() << std::endl;
    }

    std::shared_ptr<MessageBase> waitAndPop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this] { return !m_queue.empty(); });
        std::shared_ptr<MessageBase> msg = m_queue.front();
        m_queue.pop();
        std::cout << "[Queue::waitAndPop] message: " << typeid(msg).name() << std::endl;
        return msg;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::queue<std::shared_ptr<MessageBase>> m_queue;
};

}  // namespace ccy