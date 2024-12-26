#include <iostream>
using namespace std;

void func(int *a, int &b)
{
    int c;
    c = *a + b;
    printf("%d\n",c);
}

int main()
{
    int d = 2;
    int e = 4;
    func(&d, e);
    return 0;
}