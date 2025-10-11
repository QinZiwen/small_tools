#pragma once

#include "message.h"

namespace ccy {

template <typename PreviousDispatcher, typename Msg, typename Func>
class TemplateDispatcher {
  public:
    // TemplateDispatcher的各种特化版本互为友元
    template <typename D, typename M, typename F>
    friend class TemplateDispatcher;

  public:
    TemplateDispatcher(const TemplateDispatcher&)            = delete;
    TemplateDispatcher& operator=(const TemplateDispatcher&) = delete;
    TemplateDispatcher(TemplateDispatcher&& other)
        : m_queue(other.m_queue), m_prev(other.m_prev), m_func(std::move(other.m_func)), m_chained(other.m_chained) {
        other.m_chained = true;
    }
    TemplateDispatcher(Queue* q, PreviousDispatcher* prev, Func&& func)
        : m_queue(q), m_prev(prev), m_func(std::forward<Func>(func)), m_chained(false) {
        prev->m_chained = true;
        std::cout << "[TemplateDispatcher::TemplateDispatcher] queue_address: " << m_queue << std::endl;
    }
    ~TemplateDispatcher() noexcept(false) {
        std::cout << "TemplateDispatcher::~TemplateDispatcher m_chained: " << m_chained << std::endl;
        if (!m_chained) {
            waitAndDispatch();
        }
    }

    template <typename OtherMsg, typename OtherFunc>
    TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc> handle(OtherFunc&& of) {
        std::cout << "[TemplateDispatcher::handle] OtherMsg: " << typeid(OtherMsg).name()
                  << " OtherFunc: " << typeid(OtherFunc).name() << std::endl;
        return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>(m_queue, this, std::forward<OtherFunc>(of));
    }

  private:
    void waitAndDispatch() {
        std::cout << "[TemplateDispatcher::waitAndDispatch]" << std::endl;
        while (true) {
            auto msg = m_queue->waitAndPop();
            if (dispatch(msg)) {
                break;
            }
        }
        std::cout << "[TemplateDispatcher::waitAndDispatch] done" << std::endl;
    }

    bool dispatch(const std::shared_ptr<MessageBase>& msg) {
        std::cout << "[TemplateDispatcher::dispatch]" << std::endl;
        if (WrappedMessage<Msg>* wrapper = dynamic_cast<WrappedMessage<Msg>*>(msg.get())) {
            std::cout << "[TemplateDispatcher::dispatch] wrapper: " << wrapper << std::endl;
            m_func(wrapper->contents);
            return true;
        } else {
            std::cout << "[TemplateDispatcher::dispatch] wrapper: nullptr" << std::endl;
            return m_prev->dispatch(msg);
        }
    }

  private:
    Queue* m_queue;
    PreviousDispatcher* m_prev;
    Func m_func;
    bool m_chained;
};

}  // namespace ccy