#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#include "sim.h"

struct Prisoner {
    int sub_count = 0;
    int main_count = 0;
    int t1 = 1;
    int t2 = 0;
    bool is_main_counter = false;
    int num_sub_counter_tokens = 0;

    void convert_to_sub_counter(int sub_counter_quota, int stage = 1) {
        int old_quotas_reached = sub_count / sub_counter_quota;
        sub_count += t1;
        int new_quotas_reached = sub_count / sub_counter_quota;
        t1 = 0;
        num_sub_counter_tokens += 1;
        t2 += new_quotas_reached - old_quotas_reached;
        if (stage == 0) {
            is_main_counter = true;
        }
    }

    bool is_sub_counter() const {
        return num_sub_counter_tokens > 0;
    }

    bool is_under_quota(int sub_counter_quota) const {
        return sub_count < num_sub_counter_tokens * sub_counter_quota;
    }
};

int num_prisoners = Config().num_prisoners;
std::mt19937 rng(std::random_device{}());
std::uniform_int_distribution<int> dist(0, num_prisoners - 1);

std::vector<Prisoner> setup_prisoners(const Config& config) {
    std::vector<Prisoner> prisoner_list(config.num_prisoners);
    return prisoner_list;
}

bool run_stage_zero(std::vector<Prisoner>& prisoner_list, int sub_counter_quota, const Config& config) {
    bool bulb_on = false;
    for (int stage = 0; stage < config.num_sub_counters; ++stage) {
        for (int day = 0; day < config.stage_0_sub_length; ++day) {
            Prisoner& chosen_prisoner = prisoner_list[dist(rng)];
            if (day == 0) {
                if (bulb_on) {
                    chosen_prisoner.t1 += config.stage_0_sub_length - 1;
                    chosen_prisoner.convert_to_sub_counter(sub_counter_quota, stage);
                }
                if (chosen_prisoner.is_sub_counter()) {
                    chosen_prisoner.sub_count -= 1;
                    bulb_on = true;
                } else if (chosen_prisoner.t1 > 0) {
                    chosen_prisoner.t1 -= 1;
                    bulb_on = true;
                } else {
                    bulb_on = false;
                    chosen_prisoner.convert_to_sub_counter(sub_counter_quota, stage);
                }
            } else if (day == config.stage_0_sub_length - 1) {
                if (bulb_on && !chosen_prisoner.is_sub_counter()) {
                    chosen_prisoner.t1 += config.stage_0_sub_length - 1;
                    chosen_prisoner.convert_to_sub_counter(sub_counter_quota, stage);
                    bulb_on = false;
                }
            } else {
                if (bulb_on) {
                    if (chosen_prisoner.is_sub_counter()) {
                        chosen_prisoner.sub_count -= 1;
                    } else if (chosen_prisoner.t1 > 0) {
                        chosen_prisoner.t1 -= 1;
                    } else {
                        bulb_on = false;
                        chosen_prisoner.t1 += day;
                        chosen_prisoner.convert_to_sub_counter(sub_counter_quota, stage);
                    }
                }
            }
        }
    }
    return bulb_on;
}

int simulate_procedure(const Config& config) {
    int sub_counter_quota = config.num_prisoners / config.num_sub_counters;
    std::vector<Prisoner> prisoner_list = setup_prisoners(config);
    bool bulb_on = run_stage_zero(prisoner_list, sub_counter_quota, config);
    int num_days_taken = config.stage_0_sub_length * config.num_sub_counters;
    bool next_prisoner_is_counter = bulb_on;
    int stage_1_length = config.stage_1_length;
    int stage_2_length = config.stage_2_length;
    while (true) {
        for (int i = 0; i < stage_1_length; ++i) {
            num_days_taken += 1;
            Prisoner& chosen_prisoner = prisoner_list[dist(rng)];
            if (bulb_on && next_prisoner_is_counter) {
                chosen_prisoner.t1 += config.stage_0_sub_length - 1;
                chosen_prisoner.convert_to_sub_counter(sub_counter_quota);
                next_prisoner_is_counter = false;
                bulb_on = false;
            }
            if (!bulb_on && chosen_prisoner.t1 > 0) {
                bulb_on = true;
                chosen_prisoner.t1 -= 1;
            }
            if (bulb_on && chosen_prisoner.is_sub_counter() && chosen_prisoner.is_under_quota(sub_counter_quota)) {
                bulb_on = false;
                chosen_prisoner.sub_count += 1;
                if (chosen_prisoner.sub_count % sub_counter_quota == 0) {
                    chosen_prisoner.t2 += 1;
                }
            }
            if (i == stage_1_length - 1) {
                if (bulb_on) {
                    bulb_on = false;
                    chosen_prisoner.t1 += 1;
                }
            }
        }
        for (int i = 0; i < stage_2_length; ++i) {
            num_days_taken += 1;
            Prisoner& chosen_prisoner = prisoner_list[dist(rng)];
            if (!bulb_on && chosen_prisoner.t2 > 0) {
                bulb_on = true;
                chosen_prisoner.t2 -= 1;
            }
            if (bulb_on && chosen_prisoner.is_main_counter) {
                bulb_on = false;
                chosen_prisoner.main_count += 1;
                if (chosen_prisoner.main_count == config.num_sub_counters) {
                    return num_days_taken;
                }
            }
            if (i == stage_2_length - 1) {
                if (bulb_on) {
                    bulb_on = false;
                    chosen_prisoner.t2 += 1;
                }
            }
        }
        stage_1_length = config.secondary_stage_1_length;
        stage_2_length = config.secondary_stage_2_length;
    }
}

double estimate_mean(const Config& config) {
    auto start_time = std::chrono::steady_clock::now();
    std::vector<int> sim_results;
    sim_results.reserve(config.num_iterations);
    for (int i = 0; i < config.num_iterations; ++i) {
        sim_results.push_back(simulate_procedure(config));
    }
    double mean = std::accumulate(sim_results.begin(), sim_results.end(), 0.0) / config.num_iterations;
    double variance = std::accumulate(
        sim_results.begin(),
        sim_results.end(),
        0.0,
        [mean](double acc, int x) {return acc + (x - mean) * (x - mean);}
    );
    double std_dev = std::sqrt(variance / (config.num_iterations - 1));
    double standard_error = std_dev / std::sqrt(config.num_iterations);
    auto [min, max] = std::minmax_element(sim_results.begin(), sim_results.end());
    std::cout << std::fixed << "Mean: " << std::setprecision(0) << mean << '\n';
    std::cout << "Min: " << *min << '\n';
    std::cout << "Max: " << *max << '\n';
    std::cout << "Standard Error of Mean: " << std::setprecision(1) << standard_error << '\n';
     const auto elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start_time
    );
    std::cout << std::setprecision(3) << "Runtime: " << elapsed.count() << " seconds\n";
    return mean;
}

int main() {
    Config config;
    estimate_mean(config);
}