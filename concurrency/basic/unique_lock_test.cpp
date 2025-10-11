#include <iostream>
#include <mutex>
#include <thread>

class Test {
  public:
    void fun() {
        std::unique_lock<std::mutex> lck(m_mtx);
        if (bool(lck)) {
            std::cout << "lock succ: bool(lck)" << std::endl;
        }

        if ((bool)lck) {
            std::cout << "lock succ: (bool)lck" << std::endl;
        }

        if (lck) {
            std::cout << "lock succ: lck" << std::endl;
        }
    }

  private:
    std::mutex m_mtx;
};

int main() {
    Test test;
    std::thread thread(&Test::fun, &test);
    thread.join();
    return 0;
}
