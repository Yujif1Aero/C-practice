#include "HalfInputStream.h"
#include <iostream>
using namespace std;


// void HalfInputStream::SetBase() {
//     cin >> m_n;
//     m_n /= 2.0;
// }

void  HalfInputStream::SetBase(){
    InputStream::SetBase();
    m_n /= 2.0;
}