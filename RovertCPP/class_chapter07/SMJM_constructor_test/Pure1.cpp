#include "InputStream.h"
#include <iostream>
using namespace std;

int main() {
    InputStream istream;
    // Stream astream;

    cout << istream.Get() <<endl;
    cout << ">" << flush;
    istream.Set();
    cout << istream.Get() << endl;

    // cout << astream.Get() <<endl;
    // cout << ">" << flush;
    // astream.Set();
    // cout << astream.Get() << endl;
    
    //Stream stream; //pure virtual function だからstream classのオブジェクトは作られない．
}