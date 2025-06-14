#pragma once

#include <cstddef>
#include <functional>

class Executor {
    public:
        using Id = size_t;
    
        enum {
            Sync = 0,   // invoke directly
            Async,      // background thread pool
        };
    
    public:
        virtual void dispatch(std::function<void()> task) = 0;
    
    public:
        void mount(Id id);
        void demount(Id id);
        static Executor* get(Id id);
        
    protected:
        Executor() { }
    };