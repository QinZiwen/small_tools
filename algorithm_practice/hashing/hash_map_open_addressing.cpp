#include "hash_map_open_addressing.h"

namespace alp {

HashMapOpenAddressing::HashMapOpenAddressing() : size(0), buckets(capacity, nullptr) {}
HashMapOpenAddressing::~HashMapOpenAddressing() = default;

int HashMapOpenAddressing::hashFunc(int key) {
    return key % capacity;
}

double HashMapOpenAddressing::loadFactor() {
    return static_cast<double>(size) / capacity;
}

int HashMapOpenAddressing::findBucket(int key) {
    int index = hashFunc(key);
    int firstTombstone = -1;
    while (buckets[index] != nullptr) {
        if (buckets[index]->key == key) {
            if (firstTombstone != -1) {
                buckets[firstTombstone] = std::move(buckets[index]);
                buckets[index] = TOMBSTONE;
                return firstTombstone;
            }
            return index;
        } else {
            if (firstTombstone == -1 && buckets[index] == TOMBSTONE) {
                firstTombstone = index;
            }
            index = hashFunc(index + 1);
        }
    }

    return firstTombstone == -1 ? index : firstTombstone;
}

std::string HashMapOpenAddressing::get(int key) {
    int index = findBucket(key);
    if (buckets[index] != nullptr && buckets[index] != TOMBSTONE) {
        return buckets[index]->val;
    }
    return "";
}

void HashMapOpenAddressing::put(int key, const std::string& val) {
    if (loadFactor() > loadThres) {
        extend();
    }

    int index = findBucket(key);
    if (buckets[index] != nullptr && buckets[index] != TOMBSTONE) {
        buckets[index]->val = val;
    } else {
        buckets[index] = std::make_shared<Pair>(key, val);
        ++size;
    }
}

void HashMapOpenAddressing::remove(int key) {
    int index = findBucket(key);
    if (buckets[index] != nullptr && buckets[index] != TOMBSTONE) {
        buckets[index].reset();
        buckets[index] = TOMBSTONE;
        --size;
    }
}

void HashMapOpenAddressing::extend() {
    capacity *= extendRatio;
    buckets.resize(capacity);
}

void HashMapOpenAddressing::print() {
    for (const std::shared_ptr<Pair>& p : buckets) {
        if (p != nullptr && p != TOMBSTONE) {
            std::cout << p->key << " " << p->val << std::endl;
        } else if (p == TOMBSTONE) {
            std::cout << "TOMBSTONE" << std::endl;
        } else {
            std::cout << "nullptr" << std::endl;
        }
    }
}

}  // namespace alp

int main() {
    // 初始化哈希表
    alp::HashMapOpenAddressing hashmap;

    // 添加操作
    // 在哈希表中添加键值对 (key, val)
    hashmap.put(12836, "小哈");
    hashmap.put(15937, "小啰");
    hashmap.put(16750, "小算");
    hashmap.put(13276, "小法");
    hashmap.put(10583, "小鸭");
    std::cout << "\n添加完成后，哈希表为\nKey -> Value" << std::endl;
    hashmap.print();

    // 查询操作
    // 向哈希表中输入键 key ，得到值 val
    std::string name = hashmap.get(13276);
    std::cout << "\n输入学号 13276 ，查询到姓名 " << name << std::endl;

    // 删除操作
    // 在哈希表中删除键值对 (key, val)
    hashmap.remove(16750);
    std::cout << "\n删除 16750 后，哈希表为\nKey -> Value" << std::endl;
    hashmap.print();

    name = hashmap.get(16750);
    std::cout << "\n输入学号 16750 ，查询到姓名 " << name << std::endl;

    return 0;
}
