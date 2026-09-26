#include <ppl.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
double function_variant_4(double x) {
    const int limit = std::max(20, static_cast<int>(std::floor(20.0 * std::abs(x))));
    const double x2 = x * x;
    double result = 0.0;
    for (int k = 1; k <= limit; ++k) {
        const double k3 = static_cast<double>(k) * k * k;
        for (int j = 1; j <= limit; ++j)
            result += x2 * k * std::cos((k + j) * x) / (x2 + k3 + static_cast<double>(j) * j);
    }
    return result;
}
template <class F> double seconds(F&& f) {
    const auto start = std::chrono::steady_clock::now(); f();
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}
} // namespace

int main(int argc, char** argv) {
    const std::size_t count = argc > 1 ? std::stoull(argv[1]) : 1000ULL;
    std::vector<double> input(count), sequential(count), parallel(count);
    for (std::size_t i = 0; i < count; ++i) input[i] = 100.0 * std::cos(static_cast<double>(i + 1));
    const double ts = seconds([&] {
        for (std::size_t i = 0; i < count; ++i) sequential[i] = function_variant_4(input[i]);
    });
    const double tp = seconds([&] {
        concurrency::task_group group;
        for (std::size_t i = 0; i < count; ++i)
            group.run([&, i] { parallel[i] = function_variant_4(input[i]); });
        group.wait();
    });
    for (std::size_t i = 0; i < count; ++i) {
        const double scale = 1.0 + std::max(std::abs(sequential[i]), std::abs(parallel[i]));
        if (std::abs(sequential[i] - parallel[i]) > 1e-11 * scale) throw std::runtime_error("result mismatch");
    }
    std::cout << std::fixed << std::setprecision(6) << "N=" << count
              << ", sequential=" << ts << " s, task_group=" << tp << " s, speedup=" << ts / tp << '\n';
}
