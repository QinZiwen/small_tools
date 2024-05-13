#include "thread_pool_v1.h"

#include <iostream>

void ThreadPoolTest() {
    ccy::ThreadPool thread_pool;
    for (int i = 0; i < 10; ++i) {
        thread_pool.submit([i]() {
            std::cout << ">>> " << i << " <<< thread " << std::this_thread::get_id() << " is running" << std::endl;
        });
    }
}

int main() {
    ThreadPoolTest();
    return 0;
}