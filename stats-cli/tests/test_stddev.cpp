#include <vector>

#include "stats.hpp"
#include "test_util.hpp"

int main() {
    // All-equal values -> zero variance, so both population and sample
    // stddev are exactly 0.0 regardless of n.
    {
        std::vector<double> xs = {5.0, 5.0, 5.0, 5.0};
        CHECK(test::approx(stats::stddev(xs), 0.0));
        CHECK(test::approx(stats::stddev(xs, true), 0.0));
    }

    // Single element, population: mean = 5, deviation = 0 -> stddev = 0.0.
    {
        std::vector<double> xs = {5.0};
        CHECK(test::approx(stats::stddev(xs), 0.0));
    }

    // Single element, sample=true: n - 1 == 0, guarded to return 0.0 instead
    // of dividing by zero (which would otherwise yield NaN). Key edge case.
    {
        std::vector<double> xs = {5.0};
        CHECK(test::approx(stats::stddev(xs, true), 0.0));
    }

    // Classic textbook example: {2,4,4,4,5,5,7,9}, n = 8.
    // mean = (2+4+4+4+5+5+7+9)/8 = 40/8 = 5.
    // deviations: -3,-1,-1,-1,0,0,2,4
    // squared:     9, 1, 1, 1,0,0,4,16 -> sum = 32
    // population variance = 32/8 = 4  -> stddev = sqrt(4) = 2.0 exactly.
    // sample variance     = 32/7 ~= 4.571428571 -> stddev = sqrt(32/7) ~= 2.138089935.
    {
        std::vector<double> xs = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
        CHECK(test::approx(stats::stddev(xs), 2.0));
        CHECK(test::approx(stats::stddev(xs, true), 2.138089935, 1e-6));
    }

    // README set {1,2,...,10}, n = 10.
    // mean = 55/10 = 5.5.
    // deviations: -4.5,-3.5,-2.5,-1.5,-0.5,0.5,1.5,2.5,3.5,4.5
    // squared:    20.25,12.25,6.25,2.25,0.25,0.25,2.25,6.25,12.25,20.25
    //   -> sum = 82.5
    // population variance = 82.5/10 = 8.25 -> stddev = sqrt(8.25) ~= 2.872281323.
    // sample variance     = 82.5/9 ~= 9.166666667 -> stddev = sqrt(82.5/9) ~= 3.027650354.
    {
        std::vector<double> xs = {1.0, 2.0, 3.0, 4.0, 5.0,
                                   6.0, 7.0, 8.0, 9.0, 10.0};
        CHECK(test::approx(stats::stddev(xs), 2.872281323, 1e-6));
        CHECK(test::approx(stats::stddev(xs, true), 3.027650354, 1e-6));

        // Sample stddev applies Bessel's correction (divide by n-1 < n), so
        // it must be strictly larger than the population stddev here.
        CHECK(stats::stddev(xs, true) > stats::stddev(xs, false));
    }

    // Two-element set {1,3}.
    // mean = 2, deviations -1,+1, squared 1,1 -> sum = 2.
    // population variance = 2/2 = 1 -> stddev = sqrt(1) = 1.0 exactly.
    // sample variance     = 2/1 = 2 -> stddev = sqrt(2) ~= 1.414213562.
    {
        std::vector<double> xs = {1.0, 3.0};
        CHECK(test::approx(stats::stddev(xs), 1.0));
        CHECK(test::approx(stats::stddev(xs, true), 1.414213562, 1e-6));
    }

    return test::report("stddev");
}
