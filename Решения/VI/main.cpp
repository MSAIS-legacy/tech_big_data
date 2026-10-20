#define _USE_MATH_DEFINES
#include <ppl.h>
#include <concurrent_queue.h>
#include <concurrent_vector.h>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr int kDefaultGrid = 400;
constexpr int kDefaultTerms = 50;

struct Point {
    double x;
    double y;
    double f;
};

struct ValueDerivative {
    double f;
    double dfdy;
};

ValueDerivative evaluate(double x, double y, int terms) {
    double f = 0.0;
    double dfdy = 0.0;
    for (int k = 1; k <= terms; ++k) {
        const double k3 = static_cast<double>(k) * k * k;
        const double cos_kx = std::cos(k * x);
        for (int j = 1; j <= terms; ++j) {
            const double denominator =
                (1.0 + k3 + static_cast<double>(j) * j * j) *
                std::sqrt(1.0 + k + j);
            f += cos_kx * std::sin(j * y) / denominator;
            dfdy += cos_kx * j * std::cos(j * y) / denominator;
        }
    }
    return {f, dfdy};
}

template <class F>
double seconds(F&& f) {
    const auto start = std::chrono::steady_clock::now();
    f();
    return std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start).count();
}

void verify(std::size_t count, long double checksum,
            std::size_t expected_count, long double expected_sum) {
    if (count != expected_count ||
        std::abs(checksum - expected_sum) >
            1e-10L * (1.0L + std::abs(expected_sum))) {
        throw std::runtime_error("parallel result differs from sequential result");
    }
}
} // namespace

int main(int argc, char** argv) {
    const int grid = argc > 1 ? std::stoi(argv[1]) : kDefaultGrid;
    const int terms = argc > 2 ? std::stoi(argv[2]) : kDefaultTerms;
    if (grid < 1 || terms < 1) {
        throw std::invalid_argument("grid and terms must be greater than zero");
    }

    const double step = 2.0 * std::acos(-1.0) / grid;
    std::vector<Point> sequential;
    const double ts = seconds([&] {
        for (int ix = 0; ix < grid; ++ix) {
            for (int iy = 0; iy < grid; ++iy) {
                const double x = ix * step;
                const double y = iy * step;
                const auto value = evaluate(x, y, terms);
                if (value.dfdy <= 0.0) {
                    sequential.push_back({x, y, value.f});
                }
            }
        }
    });

    long double expected_sum = 0.0L;
    for (const auto& point : sequential) {
        expected_sum += point.f;
    }

    concurrency::concurrent_vector<Point> vector_points;
    const double tv = seconds([&] {
        concurrency::parallel_for(0, grid, [&](int ix) {
            for (int iy = 0; iy < grid; ++iy) {
                const double x = ix * step;
                const double y = iy * step;
                const auto value = evaluate(x, y, terms);
                if (value.dfdy <= 0.0) {
                    vector_points.push_back({x, y, value.f});
                }
            }
        });
    });
    long double vector_sum = 0.0L;
    for (const auto& point : vector_points) {
        vector_sum += point.f;
    }
    verify(vector_points.size(), vector_sum, sequential.size(), expected_sum);

    concurrency::concurrent_queue<Point> queue_points;
    const double tq = seconds([&] {
        concurrency::parallel_for(0, grid, [&](int ix) {
            for (int iy = 0; iy < grid; ++iy) {
                const double x = ix * step;
                const double y = iy * step;
                const auto value = evaluate(x, y, terms);
                if (value.dfdy <= 0.0) {
                    queue_points.push({x, y, value.f});
                }
            }
        });
    });

    std::size_t queue_count = 0;
    long double queue_sum = 0.0L;
    Point point{};
    while (queue_points.try_pop(point)) {
        ++queue_count;
        queue_sum += point.f;
    }
    verify(queue_count, queue_sum, sequential.size(), expected_sum);

    std::cout << std::fixed << std::setprecision(6)
              << "grid=" << grid << ", terms=" << terms << '\n'
              << "points=" << sequential.size() << '\n'
              << "sequential vector=" << ts << " s\n"
              << "concurrent_vector=" << tv << " s, speedup=" << ts / tv << '\n'
              << "concurrent_queue=" << tq << " s, speedup=" << ts / tq << '\n';
}
