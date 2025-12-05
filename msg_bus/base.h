#pragma once

#include <atomic>
#include <typeinfo>

using Handle = size_t;
inline Handle unifyHandle() {
    static std::atomic<Handle> counter = {0};
    return counter++;
}

namespace internal {
inline long hash(const char* txt) {
    long hash = 1;
    while (*txt)
        hash = hash * 31 + *txt++;
    return hash;
}
}  // namespace internal

template <typename T>
static const char* const name() {
    return typeid(T).name();
}

template <typename T>
static size_t hash() {
    const static size_t hash = internal::hash(name<T>());
    return hash;
}