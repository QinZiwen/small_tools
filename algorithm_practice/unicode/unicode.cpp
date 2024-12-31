#include <iostream>
#include <cuchar>
#include <codecvt>
#include <locale>
#include <string>

using namespace std;

void test1() {
    char utf8[] = u8"\u4f60\u597d\u5440\u5440";
    char16_t utf16[] = u"hello";
    char32_t utf32[] = U"hello";

    cout << "utf8: " << utf8 << endl;
    cout << "utf16: " << utf16 << endl;
    cout << "utf32: " << utf32 << endl;
}

void test2() {
    char16_t utf8[] = u"\u4f60\u597d\u5440\u5440";
    char mb[sizeof(utf8) * 2] = {0};

    mbstate_t s;
    // c16rtomb(mb, utf8, &s);

    cout << "utf8: " << utf8[0] << endl;
}

void test3() {
    wstring_convert<codecvt_utf8<wchar_t>> myconv;
    string mbstring = myconv.to_bytes(L"hello");
    cout << "mbstring: " << mbstring << " len: " << mbstring.length() << endl;
}

int main() {
    // test1();
    // test2();
    test3();
    return 0;
}