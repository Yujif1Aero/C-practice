#include <stdio.h>
#include <iostream>

int main (){
    using namespace std;
    int i;
    const char *a = "Hello";
    //char a[] = "Hello";
    //char a[10];
    printf("%d\n", a);
    printf("%s\n", a);
    cout << a << endl;
    for (i = 0; a[i]; i++){
        cout << a[i] << endl;
        printf("%d \n", &a[i]);
    }

}