#pragma once

#include "message.h"
#include "template_dispatcher.h"

namespace ccy {

class CloseQueue {};

class Dispatcher {
  public:
    template <typename Dispatcher, typename Msg, typename Func>
    friend class TemplateDispatcher;

  public:
    Dispatcher(const Dispatcher&)            = delete;
    Dispatcher& operator=(const Dispatcher&) = delete;

    Dispatcher(Queue* q) : m_queue(q), m_chained(false) {
        std::cout << "[Dispatcher] queue_address: " << m_queue << std::endl;
    }
    Dispatcher(Dispatcher&& other) : m_queue(other.m_queue), m_chained(other.m_chained) {
        other.m_chained = true;
    }
    ~Dispatcher() noexcept(false) {
        std::cout << "[Dispatcher::~Dispatcher]" << std::endl;
        if (!m_chained) {
            waitAndDispatch();
        }
    }

    template <typename Message, typename Func>
    TemplateDispatcher<Dispatcher, Message, Func> handle(Func&& func) {
        std::cout << "[Dispatcher::handle] Message_name: " << typeid(Message).name()
                  << " Func_name: " << typeid(Func).name() << std::endl;
        return TemplateDispatcher<Dispatcher, Message, Func>(m_queue, this, std::forward<Func>(func));
    }

  private:
    void waitAndDispatch() {
        std::cout << "[Dispatcher::waitAndDispatch]" << std::endl;
        while (true) {
            auto msg = m_queue->waitAndPop();
            dispatch(msg);
        }
    }

    bool dispatch(const std::shared_ptr<MessageBase>& msg) {
        if (dynamic_cast<WrappedMessage<CloseQueue>*>(msg.get())) {
            std::cout << "[Dispatcher::dispatch] CloseQueue" << std::endl;
            throw CloseQueue();
        }

        std::cout << "[Dispatcher::dispatch] return false" << std::endl;
        return false;
    }

  private:
    Queue* m_queue;
    bool m_chained;
};

class Sender {
  public:
    Sender() : m_queue(nullptr) {}
    explicit Sender(Queue* q) : m_queue(q) {}

    template <typename T>
    void send(const T& msg) {
        std::cout << "[Sender::send] typename: " << typeid(T).name() << std::endl;
        if (m_queue) {
            m_queue->push(msg);
        }
    }

  private:
    Queue* m_queue;
};

class Receiver {
  public:
    operator Sender() {
        std::cout << "[Receiver::operator Sender]" << std::endl;
        return Sender(&m_queue);
    }

    Dispatcher wait() {
        std::cout << "[Receiver::wait]" << std::endl;
        return Dispatcher(&m_queue);
    }

  private:
    Queue m_queue;
};

}  // namespace ccy