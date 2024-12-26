#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv)
{
    using namespace std;
    int p;
    int **a;

    int b;
    p = 100;
    *a = &p;
    cout << p << "\n" 
    << a << "\n"
    << *a << "\n" 
    << **a << endl;
}