#include <cstddef>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

int main()
{
    const std::string text = "Lisa Anna Lisa Sofia Anna Lisa Maria";
//ys     std::string text;
//ys     std::cout << "Input text: " << text << std::endl;
//ys     std::getline(std::cin, text);
    std::istringstream input(text);
    std::cout << "Input text: " << input.str() << std::endl;

    std::map<std::string, int> count;
    std::string                name;

    while (input >> name) {
        ++count[name];
    }

    std::cout << "Name counts:\n";
    for (const auto& [name, n] : count) {
        std::cout << name << " appears " << n << " time(s)\n";
    }
}
