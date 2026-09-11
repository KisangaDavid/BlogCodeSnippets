import random

NUM_ITERATIONS = 1000
NUM_PRISONERS = 100

simulated_results = []

def num_days_for_new_prisoner(counting_prisoner, flipped_light_prisoners):
    num_days = 1
    while True:
        chosen_prisoner = random.randint(1, NUM_PRISONERS)
        if chosen_prisoner in flipped_light_prisoners or chosen_prisoner == counting_prisoner:
            num_days += 1
            continue
        flipped_light_prisoners.add(chosen_prisoner)
        return num_days

def num_days_for_counter(counting_prisoner):
    num_days = 1
    while True:
        chosen_prisoner = random.randint(1, NUM_PRISONERS)
        if chosen_prisoner != counting_prisoner:
            num_days += 1
            continue
        return num_days

for i in range(0, NUM_ITERATIONS):
    flipped_light_prisoners = set()
    counting_prisoner = random.randint(1, NUM_PRISONERS)
    num_days_for_escape, counting_prisoner_count = 1, 1
    while counting_prisoner_count < NUM_PRISONERS:
        num_days_for_escape += num_days_for_new_prisoner(counting_prisoner, flipped_light_prisoners)
        num_days_for_escape += num_days_for_counter(counting_prisoner)
        counting_prisoner_count += 1
    simulated_results.append(num_days_for_escape)

mean = sum(simulated_results) / NUM_ITERATIONS
std_dev = (sum((x - mean) ** 2 for x in simulated_results) / NUM_ITERATIONS)**0.5

print(
    f"Avg: {mean:.0f}\n"
    f"Min: {min(simulated_results)}\n"
    f"Max: {max(simulated_results)}\n"
    f"Std deviation: {sum((x - mean)**2 for x in simulated_results / NUM_ITERATIONS)**0.5:.0f}"
)