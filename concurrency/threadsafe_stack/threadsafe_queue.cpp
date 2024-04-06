#include "threadsafe_queue.h"

#include <thread>
#include <iostream>
#include <random>

ccy::ThreadsafeQueue<int> g_threadsafe_queue;

int main() {
    std::thread thread1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::shared_ptr<int> value = g_threadsafe_queue.wait_and_pop();
        std::cout << "thread1: " << *value << std::endl;
    });

    std::thread thread2_1([&]() {
        for (int i = 0; i < 10; ++i) {
            g_threadsafe_queue.push(i);
            std::cout << "thread2_1: " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    std::thread thread2_2([&]() {
        for (int i = 0; i < 10; ++i) {
            ccy::ThreadsafeQueue<int> tmpQueue;
            for (int j = 0; j < 10; ++j) {
                tmpQueue.push(j * i);
                std::cout << "thread2_2: " << j * i << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            g_threadsafe_queue = std::move(tmpQueue);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    std::thread thread3([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int count = 0;
        while (true) {
            int value = 0;
            g_threadsafe_queue.try_pop(value);
            std::cout << ">>> thread3: " << value << std::endl;
            if (count++ > 10) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    thread1.join();
    thread2_1.join();
    thread2_2.join();
    thread3.join();
    return 0;
}