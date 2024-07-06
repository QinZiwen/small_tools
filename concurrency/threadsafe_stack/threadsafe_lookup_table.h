#pragma once

#include <thread>
#include <mutex>
#include <list>
#include <shared_mutex>
#include <algorithm>
#include <vector>

namespace ccy {

template<typename Key, typename Value, typename Hash = std::hash<Key>>
class ThreadsafeLookupTable {
private:
    class BucketType {
    private:
        using bucket_value = std::pair<Key, Value>;
        using bucket_data = std::list<bucket_value>;
        using bucket_iterator = typename bucket_data::iterator;

        bucket_data m_data;
        mutable std::shared_timed_mutex m_mutex;

        bucket_iterator find_entry_for(const Key& key) {
            // return std::find_if(m_data.begin(), m_data.end(), [&](const bucket_value& item) {
            //     return item.first == key;
            // });

            for (auto it = m_data.begin(); it != m_data.end(); ++it) {
                if (it->first == key) {
                    return it;
                }
            }

            return m_data.end();
        }
    
    public:
        Value value_for(const Key& key, const Value& default_value) {
            std::shared_lock<std::shared_timed_mutex> lock(m_mutex);
            const bucket_iterator it = find_entry_for(key);
            return (it == m_data.end()) ? default_value : it->second;
        }

        void add_or_update_mapping(const Key& key, const Value& value) {
            std::unique_lock<std::shared_timed_mutex> lock(m_mutex);
            const bucket_iterator it = find_entry_for(key);
            if (it == m_data.end()) {
                m_data.push_back(bucket_value(key, value));
            } else {
                it->second = value;
            }
        }

        void remove_mapping(const Key& key) {
            std::unique_lock<std::shared_timed_mutex> lock(m_mutex);
            const bucket_iterator it = find_entry_for(key);
            if (it != m_data.end()) {
                m_data.erase(it);
            }
        }
    };

    std::vector<std::unique_ptr<BucketType>> m_buckets;
    Hash m_hash_fn;

    BucketType& get_bucket(const Key& key) const {
        const std::size_t idx = m_hash_fn(key) % m_buckets.size();
        return *m_buckets[idx];
    }

public:
    using key_type = Key;
    using mapped_type = Value;
    using hash_type = Hash;

    ThreadsafeLookupTable(const std::size_t num_buckets = 19, const Hash& hash_fn = Hash())
        : m_buckets(num_buckets), m_hash_fn(hash_fn) {
        for (std::size_t i = 0; i < num_buckets; ++i) {
            m_buckets[i].reset(new BucketType);
        }
    }
    ThreadsafeLookupTable(const ThreadsafeLookupTable&) = delete;
    ThreadsafeLookupTable& operator=(const ThreadsafeLookupTable&) = delete;

    Value value_for(const Key& key, const Value& default_value = Value()) const {
        return get_bucket(key).value_for(key, default_value);
    }

    void add_or_update_mapping(const Key& key, const Value& value) {
        get_bucket(key).add_or_update_mapping(key, value);
    }

    void remove_mapping(const Key& key) {
        get_bucket(key).remove_mapping(key);
    }
};

}  // namespace ccy