#include "InputStream.h"
#include "ArrayStream.h"
#include <iostream>
using namespace std;

bool Average(Stream &stream) {
    int count;
    double avr = 0;

    for(count = 0; stream.Set(); ++count){
        avr += stream.Get();

    }
    if(count == 0){
        return false;
    }

    avr /= count;
    cout << "平均値は" << avr << "です．" << endl;
    return true;
}

int main(){
    InputStream istream;
    Average(istream);

    // static const double ARRAY[] = { 0.5, 1.5, -1 };
    // ArrayStream astream(ARRAY);
    // Average(astream);
}
