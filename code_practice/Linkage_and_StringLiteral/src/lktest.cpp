#include "../hd/lktest.h"
#include <iostream>
using namespace std;

void func2() { cout << a << '\n' << endl; }
int main() {
    // int b = 10;
    a = 1;
    int c;
    int *d;
    const char *str = "test";
    const char *str2;
    str2 = "tes\0t2";
    const char str_1[] = "test3";
    char str3 = 'a';
    int size = 4;
    int *test;
    cout << str << "\t"
         << str[1] << "\t"
         << *(str + 1)<< "\t" 
         << sizeof(str[4]) << "\t"
         << sizeof("test") << endl;
    cout << str_1 << "\t" << &str_1 << "\t" << sizeof(str_1) << endl;  
    cout << str2 << "\t" << &str2 << "\t" << sizeof(&str2) << endl; //str2は意味するのは，リテラル文字列があるポインタを示して，\0までを読みこんでstr2の変数にいれる．
    cout << size << "\t" << &size << "\t" << sizeof(size)  << "\t" << sizeof (&size) <<endl;
    cout << test << "\t" << &test << "\t" << sizeof(test)/*pointerのサイズが出る(64bitなら8.32bitなら4)*/ << "\t" << sizeof (&test) <<endl;
    cout << str_1 << "\t" << str_1[2] << "\t" << sizeof(str_1) << endl;
    cout << "\n" << endl;
    cout << &c << endl;
    cout << *d << endl;
    cout << c << endl;
    ++c;
    cout << &c << endl;
    c= c + a; 
    cout << &c << endl;
    cout << d << endl;
    //     func1();
    //     func1();
    //     a = 100;
    //     func2();
    //     func1();
}