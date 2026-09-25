import random
from types import SimpleNamespace

CONFIG = SimpleNamespace(
    num_iterations=2000,
    num_prisoners=100,
    num_sub_counters=10,
    stage_0_sub_length=4,
    stage_1_length=2000,
    stage_2_length=1500,
    secondary_stage_1_length=300,
    secondary_stage_2_length=300,
)

class Prisoner:
    def __init__(self):
        self.sub_count = 0
        self.main_count = 0
        self.t1 = 1
        self.t2 = 0
        self.is_main_counter = False
        self.num_sub_counter_tokens = 0

    def convert_to_sub_counter(self, sub_counter_quota, stage=1):
        old_quotas_reached = self.sub_count // sub_counter_quota
        self.sub_count += self.t1
        new_quotas_reached = self.sub_count // sub_counter_quota
        self.t1 = 0
        self.num_sub_counter_tokens += 1
        self.t2 += new_quotas_reached - old_quotas_reached
        if stage == 0:
            self.is_main_counter = True

    def is_sub_counter(self):
        return self.num_sub_counter_tokens > 0

    def is_under_quota(self, sub_counter_quota):
        return self.sub_count < self.num_sub_counter_tokens * sub_counter_quota

def setup_prisoners(config):
    return [Prisoner() for _ in range(config.num_prisoners)]

def run_stage_zero(prisoner_list, sub_counter_quota, config):
    bulb_on = False
    for stage in range(config.num_sub_counters):
        for day in range(config.stage_0_sub_length):
            chosen_prisoner = prisoner_list[random.randrange(0, config.num_prisoners)]
            if day == 0:
                if bulb_on:
                    chosen_prisoner.t1 += config.stage_0_sub_length - 1
                    chosen_prisoner.convert_to_sub_counter(sub_counter_quota, stage)
                if chosen_prisoner.is_sub_counter():
                    chosen_prisoner.sub_count -= 1
                    bulb_on = True
                elif chosen_prisoner.t1 > 0:
                    chosen_prisoner.t1 -= 1
                    bulb_on = True
                else:
                    bulb_on = False
                    chosen_prisoner.convert_to_sub_counter(sub_counter_quota, stage)
            elif day == config.stage_0_sub_length - 1:
                if bulb_on and not chosen_prisoner.is_sub_counter():
                    chosen_prisoner.t1 += config.stage_0_sub_length - 1
                    chosen_prisoner.convert_to_sub_counter(sub_counter_quota, stage)
                    bulb_on = False
            else:
                if bulb_on:
                    if chosen_prisoner.is_sub_counter():
                        chosen_prisoner.sub_count -= 1
                    elif chosen_prisoner.t1 > 0:
                        chosen_prisoner.t1 -= 1
                    else:
                        bulb_on = False
                        chosen_prisoner.t1 += day
                        chosen_prisoner.convert_to_sub_counter(sub_counter_quota, stage)
    return bulb_on

def simulate_procedure(config):
    sub_counter_quota = config.num_prisoners // config.num_sub_counters
    prisoner_list = setup_prisoners(config)
    bulb_on = run_stage_zero(prisoner_list, sub_counter_quota, config)
    num_days_taken = config.stage_0_sub_length * config.num_sub_counters
    next_prisoner_is_counter = bulb_on
    stage_1_length = config.stage_1_length
    stage_2_length = config.stage_2_length
    while True:
        for i in range(0, stage_1_length):
            num_days_taken += 1
            chosen_prisoner = prisoner_list[random.randrange(0, config.num_prisoners)]
            if bulb_on and next_prisoner_is_counter:
                chosen_prisoner.t1 += config.stage_0_sub_length - 1
                chosen_prisoner.convert_to_sub_counter(sub_counter_quota)
                next_prisoner_is_counter = False
                bulb_on = False
            if not bulb_on and chosen_prisoner.t1 > 0:
                bulb_on = True
                chosen_prisoner.t1 -= 1
            if bulb_on and chosen_prisoner.is_sub_counter() and chosen_prisoner.is_under_quota(sub_counter_quota):
                bulb_on = False
                chosen_prisoner.sub_count += 1
                if chosen_prisoner.sub_count % sub_counter_quota == 0:
                    chosen_prisoner.t2 += 1
            if i == stage_1_length - 1:
                if bulb_on:
                    bulb_on = False
                    chosen_prisoner.t1 += 1
        for i in range(0, stage_2_length):
            num_days_taken += 1
            chosen_prisoner = prisoner_list[
                random.randrange(0, config.num_prisoners)
            ]
            if not bulb_on and chosen_prisoner.t2 > 0:
                bulb_on = True
                chosen_prisoner.t2 -= 1
            if bulb_on and chosen_prisoner.is_main_counter:
                bulb_on = False
                chosen_prisoner.main_count += 1
                if chosen_prisoner.main_count == config.num_sub_counters:
                    return num_days_taken
            if i == stage_2_length - 1:
                if bulb_on:
                    bulb_on = False
                    chosen_prisoner.t2 += 1
        stage_1_length = config.secondary_stage_1_length
        stage_2_length = config.secondary_stage_2_length

def estimate_mean(config):
    sim_results = []
    for _ in range(config.num_iterations):
        sim_results.append(simulate_procedure(config))
    mean = sum(sim_results) / config.num_iterations
    std_dev = (sum((x - mean) ** 2 for x in sim_results) / (config.num_iterations - 1))**0.5
    print(
        f"Mean: {mean:.0f}\n"
        f"Min: {min(sim_results)}\n"
        f"Max: {max(sim_results)}\n"
        f"Standard Error of Mean: {std_dev / config.num_iterations**0.5:.1f}"
    )
    return mean

if __name__ == "__main__":
    estimate_mean(CONFIG)