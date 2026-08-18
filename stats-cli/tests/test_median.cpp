#include <optional>
#include <vector>

#include "stats.hpp"
#include "test_util.hpp"

int main() {
    // Empty input -> nullopt.
    CHECK(!stats::median({}).has_value());

    // Single element.
    {
        auto m = stats::median({42.0});
        CHECK(m.has_value());
        if (m.has_value()) CHECK(test::approx(*m, 42.0));
    }

    // Odd n, already sorted.
    {
        auto m = stats::median({1.0, 2.0, 3.0});
        CHECK(m.has_value());
        if (m.has_value()) CHECK(test::approx(*m, 2.0));
    }

    // Odd n, unsorted -> confirms internal sorting.
    {
        auto m = stats::median({3.0, 1.0, 2.0});
        CHECK(m.has_value());
        if (m.has_value()) CHECK(test::approx(*m, 2.0));
    }

    // Even n.
    {
        auto m = stats::median({1.0, 2.0, 3.0, 4.0});
        CHECK(m.has_value());
        if (m.has_value()) CHECK(test::approx(*m, 2.5));
    }

    // Even n, two elements.
    {
        auto m = stats::median({1.0, 2.0});
        CHECK(m.has_value());
        if (m.has_value()) CHECK(test::approx(*m, 1.5));
    }

    // Even n with negatives, average of middle two is 0.0.
    {
        auto m = stats::median({-4.0, -2.0, 2.0, 4.0});
        CHECK(m.has_value());
        if (m.has_value()) CHECK(test::approx(*m, 0.0));
    }

    // Duplicates.
    {
        auto m = stats::median({5.0, 5.0, 5.0, 5.0});
        CHECK(m.has_value());
        if (m.has_value()) CHECK(test::approx(*m, 5.0));
    }

    // Even n off-by-one check: middle two are 20 and 30, not 20 or 30 alone.
    {
        auto m = stats::median({10.0, 20.0, 30.0, 40.0});
        CHECK(m.has_value());
        if (m.has_value()) CHECK(test::approx(*m, 25.0));
    }

    return test::report("median");
}
