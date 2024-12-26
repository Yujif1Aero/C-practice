#include "InputStream.h"
#include <iostream>
using namespace std;

int main(){
    InputStream istream;

    cout << "<" << flush;
    istream.Set();
    cout << istream.Get() << endl;

    Stream stream;

    stream.Set();
    cout << stream.Get() << endl;
}