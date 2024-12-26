#include <iostream>
#include <string>
using namespace std;

inline void print(const string& s="") {
	std::cout << s;
}
int main(void){
    print("test");
}