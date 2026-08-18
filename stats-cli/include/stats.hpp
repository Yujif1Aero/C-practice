#pragma once

#include <optional>
#include <vector>

namespace stats {

// Arithmetic mean. Precondition: xs is non-empty.
double mean(const std::vector<double>& xs);

// Median. Returns nullopt for an empty input. For an even number of
// elements, returns the average of the two middle values.
std::optional<double> median(std::vector<double> xs);

// Standard deviation. Precondition: xs is non-empty.
// If sample is false (default), computes the population standard deviation
// (divides by n). If sample is true, computes the sample standard deviation
// using Bessel's correction (divides by n - 1); for n == 1 this would divide
// by zero, so that case returns 0.0 instead.
double stddev(const std::vector<double>& xs, bool sample = false);

}  // namespace stats
