#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> age;

    age["Lisa"] = 24;
    age["Anna"] = 31;
    age["Maria"] = 28;

    std::cout << "Lisa is " << age["Lisa"] << " years old.\n";

    std::cout << "\nEntries are printed in sorted key order:\n";
    for (const auto& [aaa, bbb] : age) {
        std::cout << aaa << " -> " << bbb << "\n";
    }

    if (age.contains("Lisa")) {
        std::cout << "\nLisa exists in the map.\n";
    }

    age.erase("Maria");

    std::cout << "\nAfter erasing Maria:\n";
    for (const auto& [name, years] : age) {
        std::cout << name << " -> " << years << "\n";
    }
}
