#include <vector>

#include "stats.hpp"
#include "test_util.hpp"

int main() {
    // Single element -> that element.
    CHECK(test::approx(stats::mean({42.0}), 42.0));

    // Simple set.
    CHECK(test::approx(stats::mean({1.0, 2.0, 3.0, 4.0, 5.0}), 3.0));

    // The README set, 1..10.
    CHECK(test::approx(
        stats::mean({1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0}),
        5.5));

    // Negative and mixed-sign values.
    CHECK(test::approx(stats::mean({-2.0, -1.0, 0.0, 1.0, 2.0}), 0.0));

    // Non-integer result.
    CHECK(test::approx(stats::mean({1.0, 2.0}), 1.5));

    // Values that stress precision a little.
    CHECK(test::approx(stats::mean({0.1, 0.2, 0.3}), 0.2));

    return test::report("mean");
}
