#include <ppl.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
class Integrand {
public:
    Integrand(double y, int product_terms) : sin2_y_(std::pow(std::sin(y), 2.0)), terms_(product_terms) {}
    double operator()(double x) const {
        double product = std::exp(-std::sqrt(std::abs(x)));
        for (int k = 0; k <= terms_; ++k) {
            const double denominator = sin2_y_ + std::exp(std::sqrt(static_cast<double>(k)))
                                               * std::cos(1.0 / (k + 1.0));
            product *= std::cos(x / denominator);
        }
        return product;
    }
private:
    double sin2_y_;
    int terms_;
};

template <class Function>
double simpson(double a, double b, int intervals, const Function& f) {
    const double h = (b - a) / (2.0 * intervals);
    double odd = 0.0, even = 0.0;
    for (int k = 1; k <= intervals; ++k) odd += f(a + (2 * k - 1) * h);
    for (int k = 1; k < intervals; ++k) even += f(a + 2 * k * h);
    return h * (f(a) + f(b) + 4.0 * odd + 2.0 * even) / 3.0;
}

template <class Function>
double parallel_simpson(double a, double b, int intervals, const Function& f) {
    const double h = (b - a) / (2.0 * intervals);
    concurrency::combinable<double> odd([] { return 0.0; });
    concurrency::combinable<double> even([] { return 0.0; });
    concurrency::parallel_for(1, intervals + 1, [&](int k) {
        odd.local() += f(a + (2 * k - 1) * h);
        if (k < intervals) even.local() += f(a + 2 * k * h);
    });
    const auto add = [](double left, double right) { return left + right; };
    return h * (f(a) + f(b) + 4.0 * odd.combine(add) + 2.0 * even.combine(add)) / 3.0;
}

template <class F> double seconds(F&& f) {
    const auto start = std::chrono::steady_clock::now(); f();
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}
} // namespace

int main(int argc, char** argv) {
    const double y = argc > 1 ? std::stod(argv[1]) : 1.0;
    const int intervals = argc > 2 ? std::stoi(argv[2]) : 200000;
    const double upper = argc > 3 ? std::stod(argv[3]) : 200.0;
    const int product_terms = argc > 4 ? std::stoi(argv[4]) : 512;
    if (intervals < 1 || upper <= 0.0 || product_terms < 0) throw std::invalid_argument("invalid parameters");
    const Integrand f(y, product_terms);
    double sequential = 0.0, parallel = 0.0;
    const double ts = seconds([&] { sequential = simpson(0.0, upper, intervals, f); });
    const double tp = seconds([&] { parallel = parallel_simpson(0.0, upper, intervals, f); });
    const double tolerance = 1e-9 * (1.0 + std::max(std::abs(sequential), std::abs(parallel)));
    if (std::abs(sequential - parallel) > tolerance) throw std::runtime_error("integral mismatch");
    std::cout << std::setprecision(12) << "F(" << y << ")=" << parallel << '\n'
              << std::fixed << std::setprecision(6) << "sequential=" << ts << " s, combinable=" << tp
              << " s, speedup=" << ts / tp << '\n';
}
