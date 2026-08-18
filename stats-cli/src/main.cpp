#include <iostream>
#include <string_view>
#include <vector>

#include "stats.hpp"

// Reads whitespace-separated numbers from stdin and prints summary statistics.
//   echo "1 2 3 4 5" | ./stats
//   echo "1 2 3 4 5" | ./stats --sample   (sample stddev, Bessel-corrected)
int main(int argc, char** argv) {
    bool sample = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string_view(argv[i]) == "--sample") {
            sample = true;
        }
    }

    std::vector<double> xs;
    double value = 0.0;
    while (std::cin >> value) {
        xs.push_back(value);
    }

    if (xs.empty()) {
        std::cerr << "No numbers read from stdin.\n";
        return 1;
    }

    std::cout << "count  = " << xs.size() << '\n';
    std::cout << "mean   = " << stats::mean(xs) << '\n';
    if (auto med = stats::median(xs)) {
        std::cout << "median = " << *med << '\n';
    }
    std::cout << "stddev = " << stats::stddev(xs, sample) << (sample ? "  (sample)\n" : "\n");
    return 0;
}
