#pragma once

struct Config {
    int num_iterations = 1000000;
    int num_prisoners = 100;
    int num_sub_counters = 10;
    int stage_0_sub_length = 4;
    int stage_1_length = 2000;
    int stage_2_length = 1500;
    int secondary_stage_1_length = 300;
    int secondary_stage_2_length = 300;
};

double estimate_mean(const Config& config);
