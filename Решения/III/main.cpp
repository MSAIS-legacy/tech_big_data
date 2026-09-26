#include <ppl.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
template <class F> double seconds(F&& f) {
    const auto start = std::chrono::steady_clock::now(); f();
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}

std::uint64_t checksum(const std::vector<double>& values) {
    std::uint64_t result = 0;
    for (double value : values) {
        std::uint64_t bits = 0;
        static_assert(sizeof(bits) == sizeof(value));
        std::memcpy(&bits, &value, sizeof(bits));
        result += bits;
    }
    return result;
}

template <class Sorter>
void benchmark(const char* name, const std::vector<double>& source, std::uint64_t reference_checksum, Sorter sorter) {
    std::vector<double> data(source);
    const double elapsed = seconds([&] { sorter(data); });
    if (!std::is_sorted(data.begin(), data.end())) throw std::runtime_error(std::string(name) + ": not sorted");
    if (checksum(data) != reference_checksum)
        throw std::runtime_error(std::string(name) + ": checksum mismatch");
    std::cout << name << ": " << elapsed << " s\n";
}
} // namespace

int main(int argc, char** argv) {
    const std::size_t count = argc > 1 ? std::stoull(argv[1]) : 50'000'000ULL;
    std::vector<double> source(count);
    for (std::size_t n = 0; n < count; ++n) {
        source[n] = std::sin(static_cast<double>(n));
    }
    const std::uint64_t reference_checksum = checksum(source);
    std::cout << std::fixed << std::setprecision(6) << "N=" << count << '\n';
    benchmark("std::sort", source, reference_checksum, [](auto& v) { std::sort(v.begin(), v.end()); });
    benchmark("parallel_sort", source, reference_checksum, [](auto& v) { concurrency::parallel_sort(v.begin(), v.end()); });
    benchmark("parallel_buffered_sort", source, reference_checksum,
              [](auto& v) { concurrency::parallel_buffered_sort(v.begin(), v.end()); });
}
