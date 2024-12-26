#include "HalfInputStream.h"
#include <iostream>
using namespace std;

int main() {
    HalfInputStream istream;

    cout << istream.Get() <<endl;
    cout << ">" << flush;
    istream.Set();
    cout << istream.Get() << endl;
    
    //Stream stream; //pure virtual function だからstream classのオブジェクトは作られない．
}


