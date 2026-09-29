#include <iostream>

int main() {
    int x = 5;

    asm volatile(
        "incl %0"
        : "+r"(x)
    );

    std::cout << x << std::endl;
}
