#include "msg_bus.h"

#include <iostream>
#include <chrono>
#include <thread>

struct msg {
    int x = 0;
    int y = 0;
};

Process callback(msg m) {
    std::cout << m.x << ", " << m.y << std::endl;
    return Process::Continue;
}

int main() {
    Handle h = Message<msg>().priority(200).subscribe(callback);

    msg m;
    m.x = 100;
    m.y = 200;

    publish(m, Executor::Async);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    Message<msg>::unsubscribe(h);
    return 0;
}