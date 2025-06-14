#pragma once

#include "callbackbase.h"
#include "executor.h"

using Handle = size_t;
using Priority = int32_t;

// Only effective for synchronous calls
enum class Process {
    Continue = 0,   // for broadcast
    Break,          // response chain
};

struct SubscribeStub;
using Token = std::initializer_list<int32_t>;
using Task = std::function<Process(SubscribeStub&, Executor*)>;
void doPublish(size_t hash, const Token& token, Executor::Id executor, const Task& task);

struct SubscribeStub {
    Token token;
    Handle handle = 0;
    Priority priority = 0;
    Executor::Id executor = Executor::Sync;
    std::shared_ptr<CallbackBase> callback;

    inline operator Priority() const { return priority; } // for comparation : <, ==, >
    bool accecpted(const Token& t) const;

    static Handle subscribe(size_t hash, SubscribeStub& subscribe);
    static void unsubscribe(size_t hash, Handle handle);

    template<typename T> 
    static void publish(T& t, const Token& token, Executor::Id executorId) {
        auto task = [&t] (SubscribeStub& subscribe, Executor* executor) -> Process {
            using Function = std::function<Process(T&)>;
            auto function = subscribe.callback->to<Function>();
            if (executor) { // async publish
                executor->dispatch([function, &t] () -> void { function(t); });
                return Process::Continue;
            } else { // sync publish
                return function(t);
            }
        };

        doPublish(hash<T>(), token, executorId, task);
    }

    template <typename T>
    static void publish(T* t, const Token& token, Executor::Id executorId) {
        auto task = [t] (SubscribeStub& subscribe, Executor* executor) -> Process {
            using Function = std::function<Process(T&)>;
            auto function = subscribe.callback->to<Function>();
            if (executor) { // async publish
                executor->dispatch([function, t] () -> void { function(*t); });
                return Process::Continue;
            } else { // sync publish
                return function(*t);
            }
        };

        doPublish(hash<T>(), token, executorId, task);
    }

    template <typename T>
    static void publish(std::shared_ptr<T> t, const Token& token, Executor::Id executorId) {
        auto task = [t] (SubscribeStub& subscribe, Executor* executor) -> Process {
            using Function = std::function<Process(T&)>;
            auto function = subscribe.callback->to<Function>();
            if (executor) { // async publish
                executor->dispatch([function, t] () -> void { function(*t); });
                return Process::Continue;
            } else { // sync publish
                return function(*t);
            }
        };

        doPublish(hash<T>(), token, executorId, task);
    }
};

template <typename T> class Message {
    SubscribeStub _subscribe;
public:
    Message& on(Executor::Id executor)   { _subscribe.executor = executor;   return *this; }
    Message& token(const Token& token)   { _subscribe.token = token;         return *this; }
    Message& priority(Priority priority) { _subscribe.priority = priority;   return *this; }

    /*
     * subscribe this type of message
     */
    Handle subscribe(const std::function<Process(T&)>& function) {
        _subscribe.callback = CallbackBase::from(function);
        return SubscribeStub::subscribe(hash<T>(), _subscribe);
    }
    
    /*
     * unsubscribe the specified one with same "THIS ptr & INVOKER ptr" if exist
     */
    static void unsubscribe(Handle handle) {
        SubscribeStub::unsubscribe(hash<T>(), handle);
    }
    
    /*
     * clear all the subscribers for this type of message
     */
    static void clearAllSubscribe() {
        SubscribeStub::unsubscribe(hash<T>(), 0);
    }
};

template <typename T> void publish(T& t, Executor::Id executor = Executor::Sync) {
    SubscribeStub::publish(t, { }, executor);
}

template <typename T> void publish(T& t, const Token& token, Executor::Id executor = Executor::Sync) {
    SubscribeStub::publish(t, token, executor);
}
