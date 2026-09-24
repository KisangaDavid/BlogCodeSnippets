#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>
#include <numeric>

constexpr int NUM_ITERATIONS = 1000000;
constexpr int NUM_PRISONERS = 100;
constexpr int PHASE_ONE_LENGTH = 100;

std::mt19937 rng(std::random_device{}());
std::uniform_int_distribution<int> dist(0, NUM_PRISONERS - 1);

int num_days_for_new_prisoner(std::vector<bool>& counted_prisoners) {
    int num_days = 1;
    while (true) {
        int chosen_prisoner = dist(rng);
        if (!counted_prisoners[chosen_prisoner]) {
            counted_prisoners[chosen_prisoner] = true;
            return num_days;
        }
        num_days++;
    }
}

int num_days_for_counter(int counting_prisoner) {
    int num_days = 1;
    while (true) {
        int chosen_prisoner = dist(rng);
        if (chosen_prisoner == counting_prisoner) {
            return num_days;
        }
        num_days++;
    }
}

std::pair<std::vector<bool>, int> execute_phase_one() {
    std::vector<bool> counted_prisoners(NUM_PRISONERS, false);
    int chosen_prisoner = 0;
    for (int i = 0; i < PHASE_ONE_LENGTH; i++) {
        chosen_prisoner = dist(rng);
        if (counted_prisoners[chosen_prisoner]) {
            return {counted_prisoners, chosen_prisoner};
        }
        counted_prisoners[chosen_prisoner] = true;
    }
    return {counted_prisoners, chosen_prisoner};
}

int main() {
    auto start_time = std::chrono::steady_clock::now();
    std::vector<int> sim_results;
    sim_results.reserve(NUM_ITERATIONS);
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        auto [counted_prisoners, counting_prisoner] = execute_phase_one();
        int num_days_for_escape = PHASE_ONE_LENGTH;
        int counting_prisoner_count = 0;
        for (int prisoner = 0; prisoner <= NUM_PRISONERS - 1; prisoner++) {
            if (counted_prisoners[prisoner]) {
                counting_prisoner_count++;
            }
        }
        while (counting_prisoner_count < NUM_PRISONERS) {
            num_days_for_escape += num_days_for_new_prisoner(counted_prisoners);
            num_days_for_escape += num_days_for_counter(counting_prisoner);
            counting_prisoner_count++;
        }
        sim_results.push_back(num_days_for_escape);
    }
    
    double mean = std::accumulate(sim_results.begin(), sim_results.end(), 0.0) / NUM_ITERATIONS;
    double variance = std::accumulate(
        sim_results.begin(),
        sim_results.end(),
        0.0,
        [mean](double acc, int x) {return acc + (x - mean) * (x - mean);}
    );
    double std_dev = std::sqrt(variance / (NUM_ITERATIONS - 1));
    double standard_error = std_dev / std::sqrt(NUM_ITERATIONS);

    auto [min, max] = std::minmax_element(
        sim_results.begin(),
        sim_results.end()
    );
    std::cout << std::setprecision(0);
    std::cout << "Mean: " << mean << '\n';
    std::cout << "Min: " << *min << '\n';
    std::cout << "Max: " << *max << '\n';
    std::cout << std::setprecision(1);
    std::cout << "Standard Error of Mean: " << standard_error << '\n';
    const auto elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start_time
    );
    std::cout << std::setprecision(3) << "Runtime: " << elapsed.count() << " seconds\n";

    return 0;
}