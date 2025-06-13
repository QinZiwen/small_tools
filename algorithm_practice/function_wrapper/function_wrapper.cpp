#include <iostream>
#include <vector>
#include <functional>
#include <memory>

template <typename Func>
class FunctionWrapperImpl;

// 抽象基类
class FunctionWrapper {
public:
    virtual ~FunctionWrapper() = default;
    virtual void call() = 0;  // 统一的调用接口

    // 工厂函数
    template <typename Func>
    static std::unique_ptr<FunctionWrapper> makeFunctionWrapper(Func&& f) {
        return std::make_unique<FunctionWrapperImpl<std::decay_t<Func>>>(
            std::forward<Func>(f)
        );
    }
};

// 具体实现类
template <typename Func>
class FunctionWrapperImpl : public FunctionWrapper {
private:
    Func func;
public:
    explicit FunctionWrapperImpl(Func f) : func(std::move(f)) {}
    ~FunctionWrapperImpl() { std::cout << "dectory FunctionWrapperImpl" << std::endl; }
    
    void call() override {
        func();  // 调用存储的函数
    }
};

int main() {
    std::vector<std::unique_ptr<FunctionWrapper>> functions;
    
    // 添加不同类型的函数
    functions.push_back(FunctionWrapper::makeFunctionWrapper([]() {
        std::cout << "Lambda with no arguments" << std::endl;
    }));
    
    functions.push_back(FunctionWrapper::makeFunctionWrapper([value = 42]() {
        std::cout << "Lambda with captured value: " << value << std::endl;
    }));
    
    // 调用所有函数
    for (const auto& func : functions) {
        func->call();
    }
    
    return 0;
}