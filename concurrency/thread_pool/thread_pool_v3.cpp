#include "thread_pool_v3.h"

#include <numeric>

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    unsigned long const length = std::distance(first, last);
    if (!length)
        return init;
    std::cout << "[parallel_accumulate] length: " << length << std::endl;

    unsigned long const block_size = 25;
    unsigned long const num_blocks = (length + block_size - 1) / block_size;
    std::vector<std::future<T>> futures(num_blocks - 1);
    ccy::ThreadPool thread_pool;
    Iterator block_start = first;
    for (unsigned long i = 0; i < num_blocks - 1; ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        futures[i] = thread_pool.submit([=]() {
            // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            return std::accumulate(block_start, block_end, 0);
        });
        block_start = block_end;
    }

    T last_result = std::accumulate(block_start, last, init);
    T result = std::accumulate(futures.begin(), futures.end(), last_result,
        [](T& x, std::future<T>& f) { return x + f.get(); });
    return result;
}

int main() {
    const int size = 1000000;
    std::vector<int> v(size);
    for (int i = 0; i < size; ++i) {
        v[i] = i + 1;
    }

    int res = parallel_accumulate(v.begin(), v.end(), 0);
    std::cout << "res: " << res << std::endl;
    return 0;
}