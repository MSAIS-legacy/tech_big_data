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

constexpr std::size_t kCount = 2000;

double function_variant_4(double x) {
    const int limit =
        std::max(20, static_cast<int>(std::floor(20.0 * std::abs(x))));

    const double x2 = x * x;
    double result = 0.0;

    for (int k = 1; k <= limit; ++k) {
        const double k3 =
            static_cast<double>(k) * k * k;

        for (int j = 1; j <= limit; ++j) {
            result +=
                x2 * k * std::cos((k + j) * x) /
                (x2 + k3 + static_cast<double>(j) * j);
        }
    }

    return result;
}

template <class F>
double seconds(F&& action) {
    const auto start = std::chrono::steady_clock::now();

    action();

    const auto finish = std::chrono::steady_clock::now();

    return std::chrono::duration<double>(
        finish - start
    ).count();
}

template <class A, class B>
void verify(const A& left, const B& right) {
    if (left.size() != right.size()) {
        throw std::runtime_error("size mismatch");
    }

    auto a = left.begin();
    auto b = right.begin();

    for (; a != left.end(); ++a, ++b) {
        const double scale =
            1.0 + std::max(std::abs(*a), std::abs(*b));

        if (std::abs(*a - *b) > 1e-11 * scale) {
            throw std::runtime_error("result mismatch");
        }
    }
}

template <class Container>
void test_parallel_for_each(
    const std::vector<double>& input,
    const std::string& container_name
) {
    Container sequential(input.begin(), input.end());
    Container parallel(input.begin(), input.end());

    const double seq_time = seconds([&] {
        std::for_each(
            sequential.begin(),
            sequential.end(),
            [](double& x) {
                x = function_variant_4(x);
            }
        );
    });

    const double par_time = seconds([&] {
        concurrency::parallel_for_each(
            parallel.begin(),
            parallel.end(),
            [](double& x) {
                x = function_variant_4(x);
            }
        );
    });

    verify(sequential, parallel);

    std::cout
        << "parallel_for_each (" << container_name << "): "
        << "sequential=" << seq_time << " s, "
        << "parallel=" << par_time << " s, "
        << "speedup=" << seq_time / par_time
        << '\n';
}

} // namespace


int main() {
    std::cout
        << std::fixed
        << std::setprecision(6);

    std::vector<double> input(kCount);

    for (std::size_t i = 0; i < kCount; ++i) {
        input[i] =
            100.0 * std::cos(static_cast<double>(i + 1));
    }
    
    std::vector<double> sequential(kCount);
    std::vector<double> parallel(kCount);

    const double seq_for = seconds([&] {
        for (std::size_t i = 0; i < kCount; ++i) {
            sequential[i] =
                function_variant_4(input[i]);
        }
    });

    const double par_for = seconds([&] {
        concurrency::parallel_for<std::size_t>(
            0,
            kCount,
            [&](std::size_t i) {
                parallel[i] =
                    function_variant_4(input[i]);
            }
        );
    });

    verify(sequential, parallel);

    std::cout
        << "parallel_for: "
        << "sequential=" << seq_for << " s, "
        << "parallel=" << par_for << " s, "
        << "speedup=" << seq_for / par_for
        << '\n';

    test_parallel_for_each<std::vector<double>>(
        input,
        "vector"
    );

    test_parallel_for_each<std::list<double>>(
        input,
        "list"
    );

    test_parallel_for_each<std::deque<double>>(
        input,
        "deque"
    );
    return 0;
}
