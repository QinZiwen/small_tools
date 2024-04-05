#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace alp {

struct Pair {
public:
    int key;
    std::string val;

    Pair(int key, std::string val) {
        this->key = key;
        this->val = val;
    }
};

class HashMapOpenAddressing {
public:
    HashMapOpenAddressing();
    ~HashMapOpenAddressing();

    int hashFunc(int key);
    double loadFactor();
    int findBucket(int key);
    std::string get(int key);
    void put(int key, const std::string& val);
    void remove(int key);
    void extend();
    void print();

private:
    int size;                             // 键值对数量
    int capacity = 4;                     // 哈希表容量
    const double loadThres = 2.0 / 3.0;     // 触发扩容的负载因子阈值
    const int extendRatio = 2;            // 扩容倍数
    std::vector<std::shared_ptr<Pair>> buckets;               // 桶数组
    std::shared_ptr<Pair> TOMBSTONE = std::make_shared<Pair>(-1, "-1"); // 删除标记
};

}  // namespace alp