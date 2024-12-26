#include "ArrayStream.h"
#include <iostream>
using namespace std;

ArrayStream::ArrayStream(const double* array){
    m_array = array;
//    cout << array[1] << endl;
    m_i = 0;
}

// double ArrayStream::Get() const {
//     return m_n;
// }

bool ArrayStream::Set() {
    m_n = m_array[m_i];
    //cout << m_n << endl;
    if(m_n >= 0) {
        ++m_i;
        return true;
        
    } else {
        return false;
    }
}