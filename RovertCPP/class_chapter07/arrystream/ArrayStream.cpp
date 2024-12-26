#include "ArrayStream.h"
#include <iostream>
#include <algorithm>
using namespace std;

ArrayStream::ArrayStream(const double *array, int size){
    cout << "arrystream constructor" << endl;
    m_array = new double[size];
    copy(array, array + size, m_array);
    m_i = 0;
}

ArrayStream::~ArrayStream() {
    cout << "~ArrayStream" << endl;
    delete [] m_array;
}

void ArrayStream::SetBase() {
    m_n = m_array[m_i];
    if(m_n >= 0) {
        ++m_i;
    }

}