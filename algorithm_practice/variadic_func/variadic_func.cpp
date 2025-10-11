#include <iostream>
using namespace std;

struct A {
    int a;
    A() : a(1) {}
    A(const A& a) {
        cout << "copy constructed " << __func__ << endl;
    }
    A(A&& a) {
        cout << "move constructed " << __func__ << endl;
    }
};

struct B {
    int b;
    B() : b(2) {}
    B(const B& b) {
        cout << "copy constructed " << __func__ << endl;
    }
    B(B&& b) {
        cout << "move constructed " << __func__ << endl;
    }
};

template <typename... Args>
struct MultiTypes;
template <>
struct MultiTypes<> {
    MultiTypes<>() {
        cout << "MultiTypes<>()" << endl;
    }
};
template <typename T, typename... Args>
struct MultiTypes<T, Args...> : MultiTypes<Args...> {
    MultiTypes(T t, Args... args) : MultiTypes<Args...>(args...), t(t) {
        cout << "MultiTypes(T t, Args... args)" << endl;
    }
    T t;
};

template <template <typename... Args> class VariadicType, typename... Args>
VariadicType<Args...> make_variadic(Args&&... args) {
    return VariadicType<Args...>(std::forward<Args>(args)...);
}

template <typename U, typename T, typename... Args>
U get(MultiTypes<T, Args...>& mt);

template <typename U>
U get(MultiTypes<U>& mt);

// 辅助函数定义
template <typename U, typename T, typename... Args>
U get(MultiTypes<T, Args...>& mt) {
    return get<U>(static_cast<MultiTypes<Args...>&>(mt));
}

template <typename U>
U get(MultiTypes<U>& mt) {
    return mt.t;
}

int main() {
    A a;
    B b;
    MultiTypes<A&, B&> mta = make_variadic<MultiTypes>(a, b);
    cout << mta.t.a << endl;

    // B b_ref = get<B>(mta);
    // cout << b_ref.b << endl;
    return 0;
}