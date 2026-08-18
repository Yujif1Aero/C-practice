#pragma once

#include <cmath>
#include <cstdio>

// Minimal header-only test helpers for the stats-cli unit tests.
// Each test_*.cpp is its own translation unit with its own main() that runs a
// batch of CHECK() assertions and ends with `return test::report("<name>");`.

namespace test {

// Failure counter for the current translation unit (inline => one per binary).
inline int g_failures = 0;

// Approximate equality for doubles (the stats functions return floating point).
inline bool approx(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) <= eps;
}

// Reports the tally for one test binary and yields a process exit code:
// 0 when everything passed, 1 otherwise.
inline int report(const char* name) {
    if (g_failures == 0) {
        std::printf("[PASS] %s\n", name);
        return 0;
    }
    std::printf("[FAIL] %s: %d failure(s)\n", name, g_failures);
    return 1;
}

}  // namespace test

// Records a failure (with expression and line) when `cond` is false.
#define CHECK(cond)                                                       \
    do {                                                                  \
        if (!(cond)) {                                                    \
            std::printf("  FAIL: %s (line %d)\n", #cond, __LINE__);       \
            ++test::g_failures;                                           \
        }                                                                 \
    } while (0)
