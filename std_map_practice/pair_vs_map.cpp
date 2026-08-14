#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

int main()
{
    std::pair<std::string, int> one_person = {"Yuji", 33};

    std::cout << "std::pair: one pair of values\n";
    std::cout << one_person.first << " -> " << one_person.second << "\n";

    std::map<std::string, int> age_by_name = {
        {"Yuji", 33},
        {"Lisa", 24},
        {"Anna", 31},
        {"Maria", 28},
    };

    std::cout << "\nstd::map: many pairs, sorted by key\n";
    for (const std::pair<const std::string, int>& item : age_by_name) {
        std::cout << item.first << " -> " << item.second << "\n";
    }

    std::cout << "\nstd::map: direct lookup by key\n";
    std::cout << "Lisa -> " << age_by_name.at("Lisa") << "\n";

    std::vector<std::pair<std::string, int>> ages_in_insert_order = {
        {"Yuji", 33},
        {"Lisa", 24},
        {"Anna", 31},
        {"Maria", 28},
    };

    std::cout << "\nstd::vector<std::pair>: many pairs, original order\n";
    for (const std::pair<std::string, int>& item : ages_in_insert_order) {
        std::cout << item.first << " -> " << item.second << "\n";
    }
}
