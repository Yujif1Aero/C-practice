#include <iostream>
#include <string>

using namespace std;
// string get();

string get(const bool print_info = false) {
    string r = "hello world";
    r = r + "C++";
    return r;
}
//inline void print_info() { cout << "True" ;}

int main() {
    bool a = 0;
    cout << get() << endl;
}