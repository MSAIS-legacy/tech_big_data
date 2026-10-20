#include <ppl.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
constexpr std::size_t kDefaultCount = 50'000'000;
constexpr double kScale = 4.0e9;

using Value = std::uint32_t;

struct Fingerprint {
    std::uint64_t sum = 0;
    Value xor_value = 0;
};

struct BenchmarkResult {
    std::string name;
    double seconds;
};

std::vector<Value> make_sequence(std::size_t count) {
    std::vector<Value> values(count);
    for (std::size_t n = 0; n < count; ++n) {
        values[n] = static_cast<Value>(
            kScale * std::abs(std::sin(static_cast<double>(n))));
    }
    return values;
}

Fingerprint fingerprint(const std::vector<Value>& values) {
    Fingerprint result;
    for (const Value value : values) {
        result.sum += value;
        result.xor_value ^= value;
    }
    return result;
}

void verify(const std::vector<Value>& values, const Fingerprint& expected) {
    if (!std::is_sorted(values.begin(), values.end())) {
        throw std::runtime_error("the result is not sorted");
    }

    const Fingerprint actual = fingerprint(values);
    if (actual.sum != expected.sum || actual.xor_value != expected.xor_value) {
        throw std::runtime_error("sorting changed the input values");
    }
}

template <class Sorter>
BenchmarkResult benchmark(const std::string& name,
                          const std::vector<Value>& source,
                          const Fingerprint& expected,
                          Sorter&& sorter) {
    // Copying is intentionally excluded from the measured sorting time.
    std::vector<Value> values = source;

    const auto start = std::chrono::steady_clock::now();
    std::forward<Sorter>(sorter)(values);
    const double elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start).count();

    verify(values, expected);
    return {name, elapsed};
}

void print_result(const BenchmarkResult& result, double sequential_seconds) {
    std::cout << std::left << std::setw(36) << result.name
              << std::right << std::setw(12) << result.seconds << " s";

    if (sequential_seconds > 0.0 && result.seconds > 0.0) {
        std::cout << "  speedup: " << sequential_seconds / result.seconds;
    }
    std::cout << '\n';
}
} // namespace

int main(int argc, char** argv) {
    try {
        const std::size_t count =
            argc > 1 ? static_cast<std::size_t>(std::stoull(argv[1]))
                     : kDefaultCount;
        if (count == 0) {
            throw std::invalid_argument("N must be greater than zero");
        }

        std::cout << "Variant 26 -> formula 2: x_n = 4e9 * |sin(n)|\n"
                  << "N = " << count << "\n"
                  << "Generating the source sequence...\n";

        const std::vector<Value> source = make_sequence(count);
        const Fingerprint expected = fingerprint(source);

        std::vector<BenchmarkResult> results;
        results.reserve(4);

        results.push_back(benchmark(
            "std::sort", source, expected,
            [](std::vector<Value>& values) {
                std::sort(values.begin(), values.end());
            }));

        results.push_back(benchmark(
            "concurrency::parallel_sort", source, expected,
            [](std::vector<Value>& values) {
                concurrency::parallel_sort(values.begin(), values.end());
            }));

        results.push_back(benchmark(
            "concurrency::parallel_buffered_sort", source, expected,
            [](std::vector<Value>& values) {
                concurrency::parallel_buffered_sort(values.begin(), values.end());
            }));

        results.push_back(benchmark(
            "concurrency::parallel_radixsort", source, expected,
            [](std::vector<Value>& values) {
                concurrency::parallel_radixsort(values.begin(), values.end());
            }));

        std::cout << "\nSorting time only (copying and verification excluded):\n"
                  << std::fixed << std::setprecision(6);
        const double sequential_seconds = results.front().seconds;
        for (const auto& result : results) {
            print_result(result, sequential_seconds);
        }
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
