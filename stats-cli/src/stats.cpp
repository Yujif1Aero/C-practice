#include "stats.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace stats {

double mean(const std::vector<double>& xs) {
    double sum = std::accumulate(xs.begin(), xs.end(), 0.0);
    return sum / static_cast<double>(xs.size());
}

std::optional<double> median(std::vector<double> xs) {
    if (xs.empty()) {
        return std::nullopt;
    }
    std::sort(xs.begin(), xs.end());
    const std::size_t n = xs.size();
    const std::size_t mid = n / 2;
    if (n % 2 == 1) {
        return xs[mid];
    }
    return (xs[mid - 1] + xs[mid]) / 2.0;
}

double stddev(const std::vector<double>& xs, bool sample) {
    if (sample && xs.size() == 1) {
        return 0.0;
    }
    const double m = mean(xs);
    double acc = 0.0;
    for (double x : xs) {
        const double d = x - m;
        acc += d * d;
    }
    const double divisor = sample ? static_cast<double>(xs.size() - 1)
                                   : static_cast<double>(xs.size());
    return std::sqrt(acc / divisor);
}

}  // namespace stats
