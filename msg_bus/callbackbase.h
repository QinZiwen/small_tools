//
//  ni_callback.h
//  MessageBus
//
//  Created by BernieZhao on 2023/10/24.
//

#pragma once

#include "base.h"

#include <functional>
#include <memory>

class CallbackBase {
    void* _function = nullptr;
    CallbackBase() {}

  public:
    virtual ~CallbackBase() {}

    template <typename T>
    static std::shared_ptr<CallbackBase> from(const std::function<T>& function) {
        struct C : public CallbackBase {
            C(const std::function<T>& function) {
                _function = new std::function<T>(function);
            }
            virtual ~C() override {
                delete (std::function<T>*)_function;
            }
        };
        return std::make_shared<C>(function);
    }

    template <typename T>
    T& to() const {
        return *(T*)_function;
    }
};

// class FunctionWrapper {
//     public:
//         virtual ~FunctionWrapper() = default;
//         virtual void call() = 0;  // 统一的调用接口

//         // 工厂函数
//         template <typename Func>
//         static std::unique_ptr<FunctionWrapper> makeFunctionWrapper(Func&& f) {
//             return std::make_unique<FunctionWrapperImpl<std::decay_t<Func>>>(
//                 std::forward<Func>(f)
//             );
//         }
//     };

//     // 具体实现类
//     template <typename Func>
//     class FunctionWrapperImpl : public FunctionWrapper {
//     private:
//         Func func;
//     public:
//         explicit FunctionWrapperImpl(Func f) : func(std::move(f)) {}
//         ~FunctionWrapperImpl() { std::cout << "dectory FunctionWrapperImpl" << std::endl; }

//         void call() override {
//             func();  // 调用存储的函数
//         }
//     };