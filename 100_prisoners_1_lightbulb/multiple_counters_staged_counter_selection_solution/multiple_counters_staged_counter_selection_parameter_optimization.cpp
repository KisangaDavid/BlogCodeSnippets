#include "multiple_counters_staged_counter_selection_sim.h"

#include <dlib/global_optimization.h>

#include <iomanip>
#include <iostream>
#include <vector>

constexpr int NUM_ITERATIONS = 10000;
constexpr int NUM_PRISONERS = 100;
constexpr int NUM_SUB_COUNTERS = 10;
constexpr int NUM_CALLS = 1000;

double objective(dlib::matrix<double, 0, 1> x) {
    Config config;
    config.num_iterations = NUM_ITERATIONS;
    config.num_sub_counters = NUM_SUB_COUNTERS;
    config.stage_0_sub_length = static_cast<int>(x(0));
    config.stage_1_length = static_cast<int>(x(1));
    config.stage_2_length = static_cast<int>(x(2));
    config.secondary_stage_1_length = static_cast<int>(x(3));
    config.secondary_stage_2_length = static_cast<int>(x(4));

    return estimate_mean(config);
}

int main() {
    dlib::matrix<double, 0, 1> lower(5);
    dlib::matrix<double, 0, 1> upper(5);
    lower(0) = 3;
    lower(1) = 250;
    lower(2) = 250;
    lower(3) = 50;
    lower(4) = 50;
    upper(0) = 5;
    upper(1) = 3000;
    upper(2) = 3000;
    upper(3) = 500;
    upper(4) = 500;
    std::vector<bool> is_integer(5, true);
    auto result = dlib::find_min_global(
        objective,
        lower,
        upper,
        is_integer,
        dlib::max_function_calls(NUM_CALLS)
    );
    const auto& x = result.x;
    std::cout << std::setprecision(0);
    std::cout << "Number of sub-counters: " << NUM_SUB_COUNTERS << '\n';
    std::cout << "Stage 0 sub length: " << static_cast<int>(x(0)) << '\n';
    std::cout << "Stage 1 length: " << static_cast<int>(x(1)) << '\n';
    std::cout << "Stage 2 length: " << static_cast<int>(x(2)) << '\n';
    std::cout << "Subsequent stage 1 length: " << static_cast<int>(x(3)) << '\n';
    std::cout << "Subsequent stage 2 length: " << static_cast<int>(x(4)) << '\n';
    std::cout << "Estimated mean: " << result.y << '\n';

    return 0;
}
