#include <iostream>
#include <vector>
#include <utility>

int calcGuard(std::vector<int>& arr, int left, int right)
{
    int pivot = (left + right) / 2;
    std::swap(arr[left], arr[pivot]);
    pivot = left;
    std::cout << "pivot: " << pivot << ", left: " << left << std::endl;
    while (left < right)
    {
        while (left < right && arr[right] >= arr[pivot]) {
            --right;
        }
        while (left < right && arr[left] <= arr[pivot]) {
            ++left;
        }
        std::swap(arr[left], arr[right]);
    }

    std::cout << "left: " << left << std::endl;
    std::swap(arr[pivot], arr[left]);
    return left;
}

void quickSort(std::vector<int>& arr, int left, int right)
{
    if (left >= right)
        return;
    
    std::cout << "------->> sort <<-------" << std::endl;
    int guard = calcGuard(arr, left, right);
    std::cout << "guard: " << guard << ", left: " << left << ", right: " << right << std::endl;
    for (int i = left; i <= right; ++i) {
        std::cout << " " << arr[i];
    }
    std::cout << std::endl;

    quickSort(arr, left, guard - 1);
    quickSort(arr, guard + 1, right);
}

int main() {
    std::vector<int> arr {4, 2, 6, 7, 1, 6, 9, 3};
    for (auto a : arr) {
        std::cout << " " << a;
    }
    std::cout << std::endl;

    quickSort(arr, 0, arr.size() - 1);

    for (auto a : arr) {
        std::cout << " " << a;
    }
    std::cout << std::endl;
}