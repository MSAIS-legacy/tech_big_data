#include <ppl.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr std::size_t kDefaultCount = 2000;

double function_variant_4(double x) {
    const int limit = std::max(20, static_cast<int>(std::floor(20.0 * std::abs(x))));
    const double x2 = x * x;
    double result = 0.0;
    for (int k = 1; k <= limit; ++k) {
        const double k3 = static_cast<double>(k) * k * k;
        for (int j = 1; j <= limit; ++j) {
            result += x2 * k * std::cos((k + j) * x) /
                      (x2 + k3 + static_cast<double>(j) * j);
        }
    }
    return result;
}

template <class F>
double seconds(F&& f) {
    const auto start = std::chrono::steady_clock::now();
    f();
    return std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start).count();
}

template <class Container>
void run_case(const std::string& name, const Container& input) {
    Container sequential(input.size());
    Container parallel(input.size());
    const double ts = seconds([&] {
        std::transform(input.begin(), input.end(), sequential.begin(), function_variant_4);
    });
    const double tp = seconds([&] {
        concurrency::parallel_transform(input.begin(), input.end(), parallel.begin(),
                                        function_variant_4);
    });
    auto a = sequential.begin();
    auto b = parallel.begin();
    for (; a != sequential.end(); ++a, ++b) {
        const double scale = 1.0 + std::max(std::abs(*a), std::abs(*b));
        if (std::abs(*a - *b) > 1e-11 * scale) {
            throw std::runtime_error(name + ": result mismatch");
        }
    }
    std::cout << name << ": std=" << ts << " s, parallel=" << tp
              << " s, speedup=" << ts / tp << '\n';
}
} // namespace

int main(int argc, char** argv) {
    const std::size_t count = argc > 1 ? std::stoull(argv[1]) : kDefaultCount;
    if (count == 0) {
        throw std::invalid_argument("N must be greater than zero");
    }

    std::vector<double> values(count);
    for (std::size_t i = 0; i < count; ++i) {
        values[i] = 100.0 * std::cos(static_cast<double>(i + 1));
    }
    std::cout << std::fixed << std::setprecision(6) << "N=" << count << '\n';
    run_case("vector", values);
    run_case("deque", std::deque<double>(values.begin(), values.end()));
    run_case("list", std::list<double>(values.begin(), values.end()));
}
