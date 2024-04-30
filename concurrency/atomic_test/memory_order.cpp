#include <iostream>
#include <atomic>
#include <thread>

std::atomic<int> a {0};
std::atomic<int> b {0};

void valueSet(int x) {
    a.store(x, std::memory_order_relaxed);
    b.store(2, std::memory_order_relaxed);
}

void observer() {
    std::cout << "observer [" << a << ", " << b << "]" << std::endl;
}

int main() {
    for (int i = 0; i < 5; ++i) {
        std::cout << "----------------" << std::endl;
        std::thread t1(valueSet, i);
        std::thread t0(observer);
        std::thread t2(observer);
        t0.join();
        t1.join();
        t2.join();
        std::cout << "main: " << i << " [" << a << ", " << b << "]" << std::endl;
    }
    return 0;
}