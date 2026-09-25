#pragma once

struct Config {
    int num_iterations = 1000000;
    int num_prisoners = 100;
    int num_sub_counters = 10;
    int stage_1_length = 2201;
    int stage_2_length = 1827;
    int secondary_stage_1_length = 269;
    int secondary_stage_2_length = 354;
};

double estimate_mean(const Config& config);