#include "threadsafe_stack.h"

#include <thread>
#include <iostream>
#include <random>
#include <vector>

ccy::ThreadsafeStack<int> g_safeStack;
int main() {
    std::vector<std::thread> threads;
    std::random_device rd;
    std::uniform_int_distribution<int> dist(100, 500);
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&i, &rd, &dist]() {
            std::cout << std::this_thread::get_id() << " running" << std::endl;
            for (int j = 0; j < 3; ++j) {
                g_safeStack.push(j * i);
            }

            std::cout << std::this_thread::get_id() << " popping" << std::endl;
            while (!g_safeStack.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(dist(rd)));
                try {
                    std::cout << std::this_thread::get_id() << " : " << *(g_safeStack.pop()) << std::endl;
                } catch (const ccy::EmptyStack& e) {
                    std::cout << std::this_thread::get_id() << " : " << e.what() << std::endl;
                    break;
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}