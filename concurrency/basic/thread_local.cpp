/**
 * thread_local是一个存储期指定符（storage class specifier）。
 * 与它同是存储期指定符的还有4个，分别是：auto, register, static, extern
 * 
 * thread_local 关键词只对声明于命名空间作用域的对象、声明于块作用域的对象及静态数据成员允许。
 * 它指示对象拥有线程存储期。它能与 static 或 extern 结合，以分别指定内部或外部链接（除了静态数据成员始终拥有外部链接），
 * 但附加的 static 不影响存储期。 
 * 线程存储期: 对象的存储在线程开始时分配，而在线程结束时解分配。
 * 每个线程拥有其自身的对象实例。唯有声明为 thread_local 的对象拥有此存储期。 
 * thread_local 能与 static 或 extern 一同出现，以调整链接。
*/

#include <iostream>
#include <thread>
#include <mutex>

std::mutex cout_mutex;    // 方便多线程打印
thread_local int x;       // 不初始化, 自动初始化为0
// thread_local int x = 1;

void thread_func(const std::string& thread_name) {
    for (int i = 0; i < 3; ++i) {
        x++;
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "thread[" << thread_name << "]: x = " << x << std::endl;
    }
    return;
}

/**
 * thread_local的局部变量没有因为for循环作用域而重新赋值。
 * 这是因为线程存储期的变量都是和线程绑定的，所以只有第一次声明时被赋值。
 * 可以理解为线程的static变量。不过变量的作用域依然是在本身的作用域内
*/
void thread_func_2(const std::string& thread_name) {
    for (int i = 0; i < 3; ++i) {
        thread_local int x = 1;    // 像一个静态变量，只初始化一次，并且是线程存储期
        x++;
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "thread[" << thread_name << "]: x = " << x << std::endl;
    }
    return;
}

/**
 * thread_local 为类对象，和内置类型一样
*/
class A {
public:
    A() {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "create A" << std::endl;
    }

    ~A() {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "destroy A" << std::endl;
    }

    int get_value() {
        return m_counter++;
    }

private:
    int m_counter = 0;
};

void thread_func_3(const std::string& thread_name) {
    for (int i = 0; i < 3; ++i) {
        thread_local A a;
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "thread[" << thread_name << "]: a.counter:" << a.get_value() << std::endl;
    }
    return;
}

/**
 * thread_local 为类成员变量的情况
 * 在C++中，如果您想要在类的成员变量上使用'thread_local'关键字，您需要在成员变量前面加上'static'关键字。
 * 这是因为每个对象实例都有自己的成员变量，而'thread_local'关键字指示变量具有线程局部存储期，即每个线程都有自己的变量副本。
 * 如果不使用'static'关键字，每个对象实例都将拥有自己的线程局部变量，这可能会导致混乱和不可预测的行为。
 * 通过在成员变量前添加'static'关键字，您可以确保该变量是与类相关的，而不是与特定对象实例相关的。
 * 这样，每个线程都将共享同一个变量副本，而不是为每个对象实例创建一个新的副本。
 * 因此，为了在类的成员变量上正确使用'thread_local'关键字，您需要在成员变量前加上'static'关键字，
 * 以确保变量在类的所有实例之间共享，并且在每个线程中有自己的副本。
*/
class B {
public:
    B() {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "create B" << std::endl;
    }
    ~B() {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "destroy B" << std::endl;
    }


    //thread_local static int b_key;
    static thread_local int b_key;
    int b_value = 24;
    static int b_static;
};

int B::b_static = 36;
// int B::b_key = 0;    // non-thread-local declaration of 'b_key' follows thread-local declaration
thread_local int B::b_key = 0;

void thread_func_4(const std::string& thread_name) {
    B b;
    for (int i = 0; i < 3; ++i) {
        b.b_key--;
        b.b_value--;
        b.b_static--;   // not thread safe
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "thread[" << thread_name << "]: b_key:" << b.b_key << ", b_value:" << b.b_value << ", b_static:" << b.b_static << std::endl;
        std::cout << "thread[" << thread_name << "]: B::key:" << B::b_key << ", b_value:" << b.b_value << ", b_static: " << B::b_static << std::endl;
    }
    return;
}

int main() {
    std::thread t1(thread_func, "t1");
    std::thread t2(thread_func, "t2");
    t1.join();
    t2.join();

    std::cout << "--------------------" << std::endl;
    std::thread t3(thread_func_2, "t3");
    std::thread t4(thread_func_2, "t4");
    t3.join();
    t4.join();

    std::cout << "--------------------" << std::endl;
    std::thread t5(thread_func_3, "t5");
    std::thread t6(thread_func_3, "t6");
    t5.join();
    t6.join();


    std::cout << "--------------------" << std::endl;
    std::thread t7(thread_func_4, "t7");
    std::thread t8(thread_func_4, "t8");
    t7.join();
    t8.join();
    return 0;
}
