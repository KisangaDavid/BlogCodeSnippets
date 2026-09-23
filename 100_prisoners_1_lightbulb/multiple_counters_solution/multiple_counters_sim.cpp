#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

struct Config {
    int num_iterations = 1000000;
    int num_prisoners = 100;
    int num_sub_counters = 10;
    int stage_1_length = 2252;
    int stage_2_length = 1810;
    int secondary_stage_1_length = 266;
    int secondary_stage_2_length = 257;
};

struct Prisoner {
    int sub_count = 0;
    int main_count = 0;
    int t1;
    int t2 = 0;
    bool is_main_counter;
    bool is_sub_counter;

    Prisoner(bool main_counter, bool sub_counter)
        : t1(sub_counter ? 0 : 1),
          is_main_counter(main_counter),
          is_sub_counter(sub_counter) {}
};

Config config;
std::mt19937 rng(std::random_device{}());
std::uniform_int_distribution<int> prisoner_dist(0, config.num_prisoners - 1);

std::vector<Prisoner> setup_prisoners(const Config& config) {
    std::vector<int> indices(config.num_prisoners);
    std::iota(indices.begin(), indices.end(), 0);
    std::vector<int> sub_counters(config.num_sub_counters);
    std::sample(
        indices.begin(),
        indices.end(),
        sub_counters.begin(),
        config.num_sub_counters,
        rng
    );
    int main_counter = sub_counters[0];
    std::vector<Prisoner> prisoners;
    for (int i = 0; i < config.num_prisoners; ++i) {
        bool is_sub_counter =
            std::find(sub_counters.begin(), sub_counters.end(), i) != sub_counters.end();
        bool is_main_counter = (i == main_counter);
        prisoners.emplace_back(is_main_counter, is_sub_counter);
    }
    return prisoners;
}

int simulate_procedure(const Config& config) {
    bool bulb_on = false;
    std::vector<Prisoner> prisoner_list = setup_prisoners(config);
    int num_days_taken = 0;
    int sub_counter_quota = config.num_prisoners / config.num_sub_counters - 1;
    int stage_1_length = config.stage_1_length;
    int stage_2_length = config.stage_2_length;
    while (true) {
        for (int i = 0; i < stage_1_length; ++i) {
            ++num_days_taken;
            Prisoner& chosen_prisoner = prisoner_list[prisoner_dist(rng)];
            if (!bulb_on && chosen_prisoner.t1 > 0) {
                bulb_on = true;
                --chosen_prisoner.t1;
            }
            if (bulb_on 
                && chosen_prisoner.is_sub_counter 
                && chosen_prisoner.sub_count < sub_counter_quota
            ) {
                bulb_on = false;
                ++chosen_prisoner.sub_count;
                if (chosen_prisoner.sub_count >= sub_counter_quota) {
                    ++chosen_prisoner.t2;
                }
            }
            if (i == stage_1_length - 1) {
                if (bulb_on) {
                    bulb_on = false;
                    ++chosen_prisoner.t1;
                }
            }
        }
        for (int i = 0; i < stage_2_length; ++i) {
            ++num_days_taken;
            Prisoner& chosen_prisoner = prisoner_list[prisoner_dist(rng)];
            if (!bulb_on && chosen_prisoner.t2 > 0) {
                bulb_on = true;
                --chosen_prisoner.t2;
            }
            if (bulb_on && chosen_prisoner.is_main_counter) {
                bulb_on = false;
                ++chosen_prisoner.main_count;
                if (chosen_prisoner.main_count == config.num_sub_counters) {
                    return num_days_taken;
                }
            }
            if (i == stage_2_length - 1) {
                if (bulb_on) {
                    bulb_on = false;
                    ++chosen_prisoner.t2;
                }
            }
        }
        stage_1_length = config.secondary_stage_1_length;
        stage_2_length = config.secondary_stage_2_length;
    }
}

double estimate_mean(const Config& config) {
    auto start_time = std::chrono::steady_clock::now();
    std::vector<int> simulated_results;
    simulated_results.reserve(config.num_iterations);
    for (int i = 0; i < config.num_iterations; ++i) {
        simulated_results.push_back(simulate_procedure(config));
    }

    double sum = std::accumulate(simulated_results.begin(), simulated_results.end(), 0.0);
    double mean = sum / config.num_iterations;
    double variance = 0.0;
    for (int x : simulated_results) {
        variance += (x - mean) * (x - mean);
    }
    double std_dev = std::sqrt(variance / (config.num_iterations - 1));
    double standard_error = std_dev / std::sqrt(config.num_iterations);
    auto [min, max] = std::minmax_element(
        simulated_results.begin(),
        simulated_results.end()
    );

    std::cout << std::fixed << std::setprecision(0);
    std::cout << "Mean: " << mean << '\n';
    std::cout << "Min: " << *min << '\n';
    std::cout << "Max: " << *max << '\n';
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "Standard Error of Mean: " << standard_error << '\n';
    const auto elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start_time
    );
    std::cout << std::fixed << std::setprecision(3) << "Runtime: " << elapsed.count() << " seconds\n";
    return mean;
}

int main() {
    estimate_mean(config);
    return 0;
}