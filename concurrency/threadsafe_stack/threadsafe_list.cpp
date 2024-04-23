#include "threadsafe_list.h"

#include <iostream>

int main() {
    ccy::ThreadsafeList<int> list;
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);

    std::cout << "list:";
    list.for_each([](int v) { std::cout << " " << v; });
    std::cout << std::endl;

    std::shared_ptr<int> data = list.find_first_if([](int v) { return v == 2; });
    if (data) {
        std::cout << "find 2: " << *data << std::endl;
    }

    list.remove_if([](int v) { return v == 2; });
    
    std::cout << "list:";
    list.for_each([](int v) { std::cout << " " << v; });
    std::cout << std::endl;

    return 0;
}