#pragma once

#include <list>
#include <future>
#include <type_traits>

namespace ccy {

template <typename T>
std::list<T> sequentialQuickSort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }

    std::list<T> result;
    // void splice( const_iterator pos, list& other, const_iterator it )
    // Transfers the element pointed to by it from other into *this. The element is inserted before the element pointed to by pos
    result.splice(result.begin(), input, input.begin());
    const T& pivot = *result.begin();
    // Reorders the elements in the range [first, last) in such a way that all elements for which the predicate p returns true precede all elements for which predicate p returns false. Relative order of the elements is not preserved.
    auto divide_point = std::partition(input.begin(), input.end(), [&](const T& val) { return val < pivot; });

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
    std::list<T> new_lower = sequentialQuickSort(lower_part);
    std::list<T> new_higher = sequentialQuickSort(std::move(input));
    result.splice(result.begin(), new_lower);
    result.splice(result.end(), new_higher);
    return result;
}

template <typename T>
std::list<T> parallelQuickSort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }

    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    const T& pivot = *result.begin();
    auto divide_point = std::partition(input.begin(), input.end(), [&](const T& val) { return val < pivot; });

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
    std::future<std::list<T>> new_lower = std::async(&parallelQuickSort<T>, std::move(lower_part));
    std::list<T> new_higher = parallelQuickSort(std::move(input));
    result.splice(result.begin(), new_lower.get());
    result.splice(result.end(), new_higher);
    return result;
}

template <typename F, typename A>
std::future<typename std::result_of<F(A&&)>::type> spawnTask(F&& f, A&& a) {
    typedef typename std::result_of<F(A&&)>::type result_type;
    typedef typename std::packaged_task<result_type(A&&)> task_type;
    task_type task(std::forward<F>(f));
    std::future<result_type> res = task.get_future();
    std::thread(std::move(task), std::forward<A>(a)).detach();
    return res;
}

template <typename T>
std::list<T> parallelQuickSortSpawnTask(std::list<T> input) {
    if (input.empty()) {
        return input;
    }

    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    const T& pivot = *result.begin();
    auto divide_point = std::partition(input.begin(), input.end(), [&](const T& val) { return val < pivot; });

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);
    std::future<std::list<T>> new_lower = spawnTask(&parallelQuickSort<T>, std::move(lower_part));
    std::list<T> new_higher = parallelQuickSort(std::move(input));
    result.splice(result.begin(), new_lower.get());
    result.splice(result.end(), new_higher);
    return result;
}

}  // namespace ccy