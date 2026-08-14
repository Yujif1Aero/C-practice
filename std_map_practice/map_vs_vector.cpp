#include <iostream>
#include <map>
#include <string>
#include <vector>

struct Person {
    std::string name;
    int         age;
};

int main()
{
    std::vector<Person> people = {
        { "Lisa", 24},
        { "Anna", 31},
        {"Maria", 28},
    };

    std::cout << "Vector lookup: search one by one\n";
    for (const auto& person : people) {
        if (person.name == "Lisa") {
            std::cout << "Lisa -> " << person.age << "\n";
        }
    }

    std::cout << "Test map initialization" << std::endl;
    std::map<std::string, int> age_by_name = {
        {"Yuji", 33}
    };
    for (const auto& [a, b] : age_by_name) {
        std::cout << "empty ? ->" << a << "," << b << std::endl;
    }
    for (const auto& person : people) {
        std::cout << "insert ->" << person.name << "," << person.age << std::endl;
        age_by_name[person.name] = person.age;
    }
    std::cout << "Map lookup: iterate over all items\n";
    for (const std::pair<const std::string, int>& item : age_by_name) {
        std::cout << item.first << " -> " << item.second << "\n";
    }

    std::cout << "\nMap lookup: ask directly by key\n";
    std::cout << "Lisa -> " << age_by_name.at("Lisa") << "\n";
}
