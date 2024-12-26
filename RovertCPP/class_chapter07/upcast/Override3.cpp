#include "InputStream.h"
#include <iostream>
using namespace std;

int main(){
    Stream stream;

    stream.Set();
    cout << stream.Get() << endl;

    InputStream istream;

    cout << ">" << flush;
    istream.Set();
    cout << istream.Get() << endl;
}