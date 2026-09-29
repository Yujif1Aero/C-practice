#include <iostream>

int main() {
    int a = 10;
    int b = 20;
    int result;

    asm(
        "addl %%ebx, %%eax"
        : "=a"(result)
        : "a"(a), "b"(b)
    );

    std::cout << result << std::endl;
    return 0;
}
