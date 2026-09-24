#include "multiple_counters_sim.h"

#include <dlib/global_optimization.h>

#include <iomanip>
#include <iostream>

constexpr int NUM_ITERATIONS = 5000;
constexpr int NUM_PRISONERS = 100;
constexpr int NUM_SUB_COUNTERS = 10;
constexpr int NUM_CALLS = 1000;

double objective(dlib::matrix<double, 0, 1> x) {
    Config config;
    config.num_iterations = NUM_ITERATIONS;
    config.num_prisoners = NUM_PRISONERS;
    config.num_sub_counters = NUM_SUB_COUNTERS;
    config.stage_1_length = static_cast<int>(x(0));
    config.stage_2_length = static_cast<int>(x(1));
    config.secondary_stage_1_length = static_cast<int>(x(2));
    config.secondary_stage_2_length = static_cast<int>(x(3));

    return estimate_mean(config);
}

int main() {
    dlib::matrix<double, 0, 1> lower(4);
    dlib::matrix<double, 0, 1> upper(4);
    lower(0) = 200;
    lower(1) = 200;
    lower(2) = 50;
    lower(3) = 50;
    upper(0) = 3000;
    upper(1) = 3000;
    upper(2) = 500;
    upper(3) = 500;
    std::vector<bool> is_integer(4, true);
    auto result = dlib::find_min_global(
        objective,
        lower,
        upper,
        is_integer,
        dlib::max_function_calls(NUM_CALLS)
    );
    const auto& x = result.x;
    const int stage_1_length = static_cast<int>(x(0));
    const int stage_2_length = static_cast<int>(x(1));
    const int secondary_stage_1_length = static_cast<int>(x(2));
    const int secondary_stage_2_length = static_cast<int>(x(3));
    std::cout << std::setprecision(0);
    std::cout << "Number of sub-counters: " << NUM_SUB_COUNTERS << '\n';
    std::cout << "Stage 1 length: " << stage_1_length << '\n';
    std::cout << "Stage 2 length: " << stage_2_length << '\n';
    std::cout << "Subsequent stage 1 length: " << secondary_stage_1_length << '\n';
    std::cout << "Subsequent stage 2 length: " << secondary_stage_2_length << '\n';

    return 0;
}