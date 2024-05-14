#include "thread_pool_v2.h"

#include <iostream>
#include <vector>

void ThreadPoolTest() {
    ccy::ThreadPool thread_pool;
    std::vector<std::shared_future<void>> futures;
    for (int i = 0; i < 10; ++i) {
        auto f = [i]() {
            std::cout << ">>> " << i << " <<< thread " << std::this_thread::get_id() << " is running" << std::endl;
        };
        std::future<typename std::result_of<decltype(f)()>::type> future = thread_pool.submit(f);
        futures.emplace_back(future.share());
    }

    for (auto& f : futures) {
        f.get();
    }
}

int main() {
    ThreadPoolTest();
    return 0;
}