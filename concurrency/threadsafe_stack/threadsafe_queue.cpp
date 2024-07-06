#include "threadsafe_queue.h"

#include <thread>
#include <iostream>
#include <random>
#include <vector>

ccy::ThreadsafeQueue<int> g_threadsafe_queue;
void ThreadsafeQueueTest() {
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
}

ccy::ThreadsafeQueueV2<int> g_threadsafe_queue_v2;
void ThreadsafeQueueV2Test() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 1; ++i) {
        threads.emplace_back([&]() {
            while(true) {
                std::shared_ptr<int> res = g_threadsafe_queue_v2.try_pop();
                if (res) {
                    std::cout << "thread1: " << *res << std::endl;
                } else {
                    std::cout << "thread1: res is nullptr" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        });
    }

    for (int i = 0; i < 2; ++i) {
        threads.emplace_back([&]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 100);
            for (int i = 0; i < 10; ++i) {
                int v = dis(gen);
                std::cout << std::this_thread::get_id() << " : " << v << std::endl;
                g_threadsafe_queue_v2.push(v);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

ccy::ThreadsafeQueueV3<int> g_threadsafe_queue_v3;
void ThreadsafeQueueV3Test() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([&]() {
            while(true) {
                std::shared_ptr<int> res = g_threadsafe_queue_v3.try_pop();
                if (res) {
                    std::cout << std::this_thread::get_id() << " : " << *res << std::endl;
                } else {
                    std::cout << std::this_thread::get_id() << " : res is nullptr" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        });
    }

    for (int i = 0; i < 2; ++i) {
        threads.emplace_back([&]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 100);
            while (true) {
                int v = dis(gen);
                std::cout << std::this_thread::get_id() << " : " << v << std::endl;
                g_threadsafe_queue_v3.push(v);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

ccy::ThreadsafeQueueV4<int> g_threadsafe_queue_v4;
void ThreadsafeQueueV4Test() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([&]() {
            while(true) {
                std::shared_ptr<int> res = g_threadsafe_queue_v4.wait_pop();
                std::cout << std::this_thread::get_id() << " << " << *res << std::endl;
            }
        });
    }

    for (int i = 0; i < 2; ++i) {
        threads.emplace_back([&]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 100);
            while (true) {
                int v = dis(gen);
                std::cout << std::this_thread::get_id() << " >> " << v << std::endl;
                g_threadsafe_queue_v4.push(v);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

int main() {
    // ThreadsafeQueueTest();
    // ThreadsafeQueueV2Test();
    // ThreadsafeQueueV3Test();
    ThreadsafeQueueV4Test();

    return 0;
}