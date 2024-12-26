#include "ArrayStream.h"
#include "InputStream.h"
#include <iostream>
using namespace std;

Stream *CreateStream() {
    char ch;

    while (true) {
        cout << "i : InputStream, a: ArrayStream > " << flush;
        cin >> ch;

        switch (ch) {
        case 'i':
            return new InputStream();
         case 'a':
         while(true){
            cout << "配列を選んでください(1,2) >" << flush;
            cin >>ch;
            if (ch == '1'){
                static const double ARRAY[] = { 1, 2, 3, -1};
                static const int SIZE = sizeof (ARRAY) / sizeof (*ARRAY);
                return new ArrayStream(ARRAY, SIZE);
            } else if (ch == '2') {
                static const double ARRAY[] = { 0.5, 1.5, -1};
                static const int SIZE = sizeof (ARRAY) / sizeof (*ARRAY);
                return new ArrayStream(ARRAY, SIZE);
            } else {
                return NULL;
            }
         }//while (true)
        } //switch (ch)
    } //while(true)
}


double Sum(Stream &stream) {
    double sum = 0;
    while( stream.Set()) {
        sum += stream.Get();
    }
    return sum;
}

int main() {
    for (int i = 0 ; i < 3; ++i) {
        Stream* stream = CreateStream();
        cout << i << endl;
        cout << stream << endl;
        double sum = Sum(*stream);
        cout << "合計: " << sum << endl;
        delete stream;
        cout << stream << endl;
    }
}