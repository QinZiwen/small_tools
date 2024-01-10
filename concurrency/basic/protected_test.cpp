#include <iostream>

class CBase {
public:
    int m_Public;
protected:
    int m_Protected;
private:
    int m_Private;
};

class CDerived :public CBase {
public:
    void AccessBase() {
        m_Public = 1;
        m_Protected = 2;
        // m_private = 3;

        CBase base;
        base.m_Public = 1;
        // base.nProtected = 2;    // Cannot access directly
    }
};

int main() {
    CBase base;
    base.m_Public = 1;
    // base.nProtected = 2;    // Cannot access directly

    return 0;
}