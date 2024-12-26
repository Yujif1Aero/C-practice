#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
using namespace std;
int main(int argc, char* argv[]){
std::vector<string> a(1, "0");
a[0] = "ab";

//int i;
//for(i = 0; i<1; i++){
//  cout << a[0];
printf("%s\n",a[0].c_str());
//}

return 0;
}