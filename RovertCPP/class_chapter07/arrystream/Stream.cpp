#include "Stream.h"
#include <iostream>
using namespace std;



Stream::Stream() :
    m_n(-1)
 {
    cout << "constructor of stream" << endl;
 }

Stream::Stream(double n) :
 m_n(n)
 {
    
 }

double Stream::Get() const {
    return m_n;
}

bool Stream::Set() {
    SetBase();
    cout << "Stream::Set" << endl;
    return m_n >= 0;
}

// Stream::~Stream(){
//     cout << "Dstructor of Stream" << endl;
//   }