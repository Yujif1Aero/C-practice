#include <iostream>
#include <string>

using namespace std;
// string get();

string get() {
    string r = "hello world";
    r = r + "C++";
    return r;
}
int main() {
    
    cout << get() << endl;
}