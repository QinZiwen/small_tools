#include <iostream>

int fun(int a) {
    std::cout << "a: " << a << std::endl;
    int res = a;
    for (int i = 0; i < a; ++i) {
        res += a;
        std::cout << "res: " << res << std::endl;
    }
    return res;
}

int main() {
    int a = 5;
    std::cout << "a: " << a << std::endl;
    a = fun(a);

    int b = a;
    std::cout << "b: " << b << std::endl;
    return 0;
}
