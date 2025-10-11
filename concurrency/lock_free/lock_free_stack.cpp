#include "lock_free_stack.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

ccy::LockFreeStack<int> g_stack;

void LockFreeStackTest() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 1; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 10; ++j) {
                g_stack.push(j);
                std::cout << std::this_thread::get_id() << " << " << j << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            while (true) {
                std::cout << "read stack ..." << std::endl;
                std::shared_ptr<int> data = g_stack.pop();
                if (data != nullptr) {
                    std::cout << ">> " << std::this_thread::get_id() << " >> " << *data << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                } else {
                    std::cout << "read thread break" << std::endl;
                    break;
                }
            }
        });
    }

    std::cout << "threads size: " << threads.size() << std::endl;
    for (auto& thread : threads) {
        thread.join();
    }
    std::cout << " >>>>>> end thread <<<<<<" << std::endl;
}

int main() {
    LockFreeStackTest();
    return 0;
}