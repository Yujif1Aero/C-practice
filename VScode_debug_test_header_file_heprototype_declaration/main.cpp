#define MAIN
#include <iostream>
#include "Heprototype_declaration.hpp"
int main(){

 int c = sum(1 , 2);
 std::cout << c << std::endl;

 int d = sum(1 , c);
 std::cout << d << std::endl;

}


