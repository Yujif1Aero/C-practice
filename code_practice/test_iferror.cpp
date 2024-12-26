#include <iostream>
#include <string>

using namespace std;
// string get();

void get(int* error1, int* error2) {
    *error1 = 1;
    *error2 = 0;
    //return error;
}
int main() {
    int a, b;
    get(&a, &b);
    if(a) cout << "a is " << a << endl;
    if(b) cout << "b is " << b <<endl;
    return 0;
}