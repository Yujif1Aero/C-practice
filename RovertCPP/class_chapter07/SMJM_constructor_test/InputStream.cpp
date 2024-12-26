#include "InputStream.h"
#include <iostream>
using namespace std;

InputStream::InputStream() { cout << "InputStream" << endl; }
InputStream::InputStream(double n) : Stream(n) {}

// double InputStream::Get() const{
//     return m_n;
// }

// bool InputStream::Set(){
//     cin >> m_n;
//     return m_n >=0;
void InputStream::SetBase() { cin >> m_n; }

InputStream::~InputStream() { cout << "Dstructor of InputStream" << endl; }
