#include "ArrayStream.h"
#include <iostream>
using namespace std;

int main() {
    static const double ARRAY[] = { 1, 2, 3, -1};
    static const int SIZE = sizeof ARRAY / sizeof *ARRAY;
    ArrayStream stream(ARRAY, SIZE);
    while(stream.Set()) {
        cout << stream.Get() << ' ' ;
    }
    cout << endl;
}