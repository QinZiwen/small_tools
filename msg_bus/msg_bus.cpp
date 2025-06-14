#include "msg_bus.h"

#include <mutex>
#include <unordered_map>

#include "base.h"

struct Vector : std::mutex, std::vector<SubscribeStub> { };
struct Map : std::mutex, std::unordered_map<size_t, std::shared_ptr<Vector>> { };

static Map g_map;

std::shared_ptr<Vector> getSubscribeVector(size_t hash) {
    std::lock_guard<Map> lock(g_map);
    auto& vector = g_map[hash];
    if (!vector) vector = std::make_shared<Vector>();
    return vector;
}

bool SubscribeStub::accecpted(const Token& other) const {
    if (!token.size() || !other.size()) return true;
    
    for (auto& a : token) {
        for (auto& o : other) {
            if (a == o) return true;
        }
    }
    
    return false;
}

Handle SubscribeStub::subscribe(size_t hash, SubscribeStub& subscribe) {
    std::shared_ptr<Vector> vector = getSubscribeVector(hash);
    
    { // insert subscribe with priority order
        std::lock_guard<Vector> lock(*vector);
        auto it = vector->begin();
        while (it != vector->end()) {
            if (subscribe > *it) {
                it++;
            } else {
                break;
            }
        }
        
        Handle handle = unifyHandle();
        subscribe.handle = handle;
        vector->insert(it, subscribe);
        return handle;
    }
}

void SubscribeStub::unsubscribe(size_t hash, Handle handle) {
    std::shared_ptr<Vector> vector = getSubscribeVector(hash);
    
    {
        std::lock_guard<Vector> lock(*vector);
        if (handle == 0) { // clear all the subscribes
            vector->clear();
        } else { // erase the specified callback
            auto it = vector->begin();
            while (it != vector->end()) {
                if (it->handle == handle) {
                    vector->erase(it);
                    return;
                }
                it++;
            }
        }
    }
}

void doPublish(size_t hash, const Token& token, Executor::Id executor, const Task& task) {
    std::shared_ptr<Vector> vector = getSubscribeVector(hash);
    
    std::vector<SubscribeStub> tmp;
    
    {
        std::lock_guard<Vector> lock(*vector);
        tmp = *vector;
    }

    auto it = tmp.begin();
    while (it != tmp.end()) {
        Executor* e = nullptr;
        if (it->executor != Executor::Sync) {
            e = Executor::get(it->executor);
        } else if (executor != Executor::Sync) {
            e = Executor::get(executor);
        }
        
        if (it->accecpted(token) &&
            Process::Break == task(*it, e)) {
            return;
        }
        it++;
    }
}