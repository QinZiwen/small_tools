#include "lock_free_queue.h"

#include <thread>
#include <vector>

ccy::LockFreeQueue<int> g_queue;

void LockFreeQueueTest() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 1; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 10; ++j) {
                g_queue.push(j);
                std::cout << std::this_thread::get_id() << " << " << j << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }

    for (int i = 0; i < 2; ++i) {
        threads.emplace_back([&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            while (true) { 
                std::cout << "read queue ..." << std::endl;
                std::shared_ptr<int> data = g_queue.pop();
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
    LockFreeQueueTest();
    return 0;
}