#include "../hd/lktest.h"
#include <iostream>
using namespace std;

void func1() {
    //int b = 10;
    static int b = 10;
    cout << a << '\n' << b << '\n' << endl;
   // cout << b << endl;
    func2();
    ++a;
    ++b;
}
