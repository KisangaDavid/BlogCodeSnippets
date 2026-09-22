import random

NUM_ITERATIONS = 10000
NUM_PRISONERS = 100

NUM_SUB_COUNTERS = 10
STAGE_1_LENGTH = 2252
STAGE_2_LENGTH = 1810 
SECONDARY_STAGE_1_LENGTH = 266
SECONDARY_STAGE_2_LENGTH = 257

class Prisoner:
    def __init__(self, is_main_counter, is_sub_counter):
        self.sub_count = 0
        self.main_count = 0
        self.t1 = 1 if not is_sub_counter else 0 
        self.t2 = 0
        self.is_main_counter = is_main_counter
        self.is_sub_counter = is_sub_counter

def setup_prisoners(num_sub_counters):
    prisoners = []
    sub_counters = random.sample(range(0,NUM_PRISONERS), num_sub_counters)
    main_counter = sub_counters[0]
    for i in range(0, NUM_PRISONERS):
        is_sub_counter = False
        is_main_counter = False
        if i == main_counter:
            is_main_counter = True
        if i in sub_counters:
            is_sub_counter = True
        prisoners.append(Prisoner(is_main_counter, is_sub_counter))
    return prisoners

def simulate_procedure(num_sub_counters, 
                       stage_1_length, 
                       stage_2_length, 
                       secondary_stage_1_length, 
                       secondary_stage_2_length):
    bulb_on = False
    prisoner_list = setup_prisoners(num_sub_counters)
    num_days_taken = 0
    sub_counter_quota = NUM_PRISONERS // num_sub_counters - 1
    while True:
        for i in range(0, stage_1_length):
            num_days_taken += 1
            chosen_prisoner = prisoner_list[random.randrange(0, NUM_PRISONERS)]
            if not bulb_on and chosen_prisoner.t1 > 0:
                bulb_on = True
                chosen_prisoner.t1 -= 1
            if (bulb_on 
                and chosen_prisoner.is_sub_counter 
                and chosen_prisoner.sub_count < sub_counter_quota
            ):
                bulb_on = False
                chosen_prisoner.sub_count += 1
                if chosen_prisoner.sub_count >= sub_counter_quota:
                    chosen_prisoner.t2 += 1
            if i == stage_1_length - 1:
                if bulb_on:
                    bulb_on = False
                    chosen_prisoner.t1 += 1
        for i in range(0, stage_2_length):
            num_days_taken += 1
            chosen_prisoner = prisoner_list[random.randrange(0, NUM_PRISONERS)]
            if not bulb_on and chosen_prisoner.t2 > 0:
                bulb_on = True
                chosen_prisoner.t2 -= 1
            if bulb_on and chosen_prisoner.is_main_counter:
                bulb_on = False
                chosen_prisoner.main_count += 1
                if chosen_prisoner.main_count == num_sub_counters:
                    return num_days_taken
            if i == stage_2_length - 1:
                if bulb_on:
                    bulb_on = False
                    chosen_prisoner.t2 += 1
        stage_1_length = secondary_stage_1_length
        stage_2_length = secondary_stage_2_length

def estimate_mean(num_iterations, 
                  num_sub_counters, 
                  stage_1_length, 
                  stage_2_length,
                  secondary_stage_1_length,
                  secondary_stage_2_length):
    simulated_results = []
    for _ in range(0, num_iterations):
        simulated_results.append(simulate_procedure(num_sub_counters, 
                                                    stage_1_length, 
                                                    stage_2_length,
                                                    secondary_stage_1_length,
                                                    secondary_stage_2_length))
    mean = sum(simulated_results) / num_iterations
    std_dev = (sum((x - mean) ** 2 for x in simulated_results) / num_iterations)**0.5
    print(
        f"Mean: {mean:.0f}\n"
        f"Min: {min(simulated_results)}\n"
        f"Max: {max(simulated_results)}\n"
        f"Standard Error of Mean: {std_dev / num_iterations**0.5:.1f}"
    )
    return mean

if __name__ == "__main__":
    estimate_mean(NUM_ITERATIONS, 
                  NUM_SUB_COUNTERS, 
                  STAGE_1_LENGTH, 
                  STAGE_2_LENGTH, 
                  SECONDARY_STAGE_1_LENGTH, 
                  SECONDARY_STAGE_2_LENGTH)