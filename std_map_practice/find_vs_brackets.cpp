#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> scores;
    scores["Lisa"] = 95;
    scores["Sofia"] = 88;

    std::cout << "Before operator[] lookup, size = " << scores.size() << "\n";
    std::cout << "scores[\"Nadia\"] = " << scores["Nadia"] << "\n";
    std::cout << "After operator[] lookup, size = " << scores.size() << "\n";

    const auto it = scores.find("Olena");
    if (it == scores.end()) {
        std::cout << "find(\"Olena\") did not insert a new key.\n";
    }
    std::cout << "After find lookup, size = " << scores.size() << "\n";
}
