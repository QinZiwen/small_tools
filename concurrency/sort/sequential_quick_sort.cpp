#include "sequential_quick_sort.h"

#include <iostream>

int main() {
    std::list<int> list = {4, 5, 1, 6, 9, 2, 7, 3, 8, 4, 5, 1, 6, 9, 2, 7, 3, 8};
    list                = ccy::sequentialQuickSort(list);
    for (auto i : list) {
        std::cout << i << " ";
    }
    std::cout << "\n--------" << std::endl;

    list = {4, 5, 1, 6, 9, 2, 7, 3, 8, 4, 5, 1, 6, 9, 2, 7, 3, 8};
    list = ccy::parallelQuickSort(list);
    for (auto i : list) {
        std::cout << i << " ";
    }
    std::cout << "\n--------" << std::endl;

    list = {4, 5, 1, 6, 9, 2, 7, 3, 8, 4, 5, 1, 6, 9, 2, 7, 3, 8};
    list = ccy::parallelQuickSortSpawnTask(list);
    for (auto i : list) {
        std::cout << i << " ";
    }
    std::cout << "\n--------" << std::endl;
    return 0;
}