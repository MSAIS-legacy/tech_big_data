#include <agents.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {
int g_terms = 100;

double function_1(double x) {
    const double abs_x = std::abs(x);
    double result = 0.0;
    for (int n = 0; n <= g_terms; ++n) {
        const double n3 = static_cast<double>(n) * n * n;
        for (int k = 0; k <= g_terms; ++k) {
            const double k2 = static_cast<double>(k) * k;
            for (int j = 0; j <= g_terms; ++j)
                result += (x + n) / (1.0 + abs_x + n3 + k2 + static_cast<double>(j) * j);
        }
    }
    return result;
}

double function_2(double x) {
    const double abs_x = std::abs(x);
    double result = 0.0;
    for (int n = 0; n <= g_terms; ++n) {
        const double n2 = static_cast<double>(n) * n;
        for (int k = 0; k <= g_terms; ++k) {
            const double k3 = static_cast<double>(k) * k * k;
            for (int j = 0; j <= g_terms; ++j) {
                const double radial = std::pow(n2 + static_cast<double>(j) * j, 1.5);
                result += x / (1.0 + abs_x + radial + k3);
            }
        }
    }
    return result;
}

struct Item {
    std::size_t index{};
    double value{};
    bool stop{};
};

class StageAgent final : public concurrency::agent {
public:
    StageAgent(concurrency::ISource<Item>& source, concurrency::ITarget<Item>& target,
               std::function<double(double)> function)
        : source_(source), target_(target), function_(std::move(function)) {}
private:
    void run() override {
        for (;;) {
            Item item = concurrency::receive(source_);
            if (item.stop) { concurrency::send(target_, item); break; }
            item.value = function_(item.value);
            concurrency::send(target_, item);
        }
        done();
    }
    concurrency::ISource<Item>& source_;
    concurrency::ITarget<Item>& target_;
    std::function<double(double)> function_;
};

template <class F> double seconds(F&& f) {
    const auto start = std::chrono::steady_clock::now(); f();
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}

void verify(const std::vector<double>& expected, const std::vector<double>& actual, const char* name) {
    for (std::size_t i = 0; i < expected.size(); ++i) {
        const double scale = 1.0 + std::max(std::abs(expected[i]), std::abs(actual[i]));
        if (std::abs(expected[i] - actual[i]) > 1e-11 * scale)
            throw std::runtime_error(std::string(name) + ": result mismatch");
    }
}
} // namespace

int main(int argc, char** argv) {
    g_terms = argc > 1 ? std::stoi(argv[1]) : 100;
    const std::size_t count = argc > 2 ? std::stoull(argv[2]) : 1000ULL;
    if (g_terms < 0 || count == 0) throw std::invalid_argument("invalid parameters");
    std::vector<double> input(count), sequential(count), agent_result(count), transformer_result(count);
    for (std::size_t i = 0; i < count; ++i) input[i] = 10.0 * std::sin(static_cast<double>(i));

    const double ts = seconds([&] {
        for (std::size_t i = 0; i < count; ++i) sequential[i] = function_2(function_1(input[i]));
    });

    const double ta = seconds([&] {
        concurrency::unbounded_buffer<Item> input_buffer, middle_buffer, output_buffer;
        StageAgent first(input_buffer, middle_buffer, function_1);
        StageAgent second(middle_buffer, output_buffer, function_2);
        first.start(); second.start();
        for (std::size_t i = 0; i < count; ++i) concurrency::send(input_buffer, Item{i, input[i], false});
        concurrency::send(input_buffer, Item{0, 0.0, true});
        for (std::size_t i = 0; i < count; ++i) {
            const Item item = concurrency::receive(output_buffer);
            if (item.stop) throw std::runtime_error("early pipeline stop");
            agent_result[item.index] = item.value;
        }
        const Item stop = concurrency::receive(output_buffer);
        if (!stop.stop) throw std::runtime_error("missing pipeline stop");
        concurrency::agent::wait(&first); concurrency::agent::wait(&second);
    });
    verify(sequential, agent_result, "agents");

    const double tt = seconds([&] {
        concurrency::transformer<Item, Item> first([](Item item) { item.value = function_1(item.value); return item; });
        concurrency::transformer<Item, Item> second([](Item item) { item.value = function_2(item.value); return item; });
        first.link_target(&second);
        for (std::size_t i = 0; i < count; ++i) concurrency::send(first, Item{i, input[i], false});
        for (std::size_t i = 0; i < count; ++i) {
            const Item item = concurrency::receive(second);
            transformer_result[item.index] = item.value;
        }
        first.unlink_target(&second);
    });
    verify(sequential, transformer_result, "transformers");

    std::cout << std::fixed << std::setprecision(6) << "terms=" << g_terms << ", count=" << count << '\n'
              << "sequential=" << ts << " s\n"
              << "agents=" << ta << " s, speedup=" << ts / ta << '\n'
              << "transformers=" << tt << " s, speedup=" << ts / tt << '\n';
}
