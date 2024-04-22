#include "threadsafe_lookup_table.h"
#include <iostream>
#include <random>

ccy::ThreadsafeLookupTable<int, int> g_lookup_table;
void ThreadsafeLookupTableTest() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([&]() {
            while(true) {
                for (int j = 0; j < 10; ++j) {
                    int v = g_lookup_table.value_for(j, j);
                    std::cout << std::this_thread::get_id() << " << " << v << std::endl;
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
                for (int j = 0; j < 10; ++j) {
                    int v = dis(gen);
                    std::cout << std::this_thread::get_id() << " >> " << v << std::endl;
                    g_lookup_table.add_or_update_mapping(j, v);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

int main() {
    ThreadsafeLookupTableTest();
    return 0;
}