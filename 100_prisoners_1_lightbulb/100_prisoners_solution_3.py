import random

NUM_ITERATIONS = 1000
NUM_PRISONERS = 100
PHASE_ONE_LENGTH = 100

simulated_results = []

def num_days_for_new_prisoner(counted_prisoners):
    num_days = 1
    while True:
        chosen_prisoner = random.randint(1, NUM_PRISONERS)
        if chosen_prisoner not in counted_prisoners:
            counted_prisoners.add(chosen_prisoner)
            return num_days 
        num_days += 1

def num_days_for_counter(counting_prisoner):
    num_days = 1
    while True:
        chosen_prisoner = random.randint(1, NUM_PRISONERS)
        if chosen_prisoner == counting_prisoner:
            return num_days
        num_days += 1

def execute_phase_one():
    counted_prisoners = set()
    for _ in range(0, PHASE_ONE_LENGTH):
        chosen_prisoner = random.randint(1, NUM_PRISONERS)
        if chosen_prisoner in counted_prisoners:
            return counted_prisoners, chosen_prisoner
        counted_prisoners.add(chosen_prisoner)
    return counted_prisoners, chosen_prisoner 
    
for i in range(0, NUM_ITERATIONS):
    counted_prisoners, counting_prisoner = execute_phase_one()
    num_days_for_escape = PHASE_ONE_LENGTH
    counting_prisoner_count = len(counted_prisoners)
    while counting_prisoner_count < NUM_PRISONERS:
        num_days_for_escape += num_days_for_new_prisoner(counted_prisoners)
        num_days_for_escape += num_days_for_counter(counting_prisoner)
        counting_prisoner_count += 1
    simulated_results.append(num_days_for_escape)

mean = sum(simulated_results) / NUM_ITERATIONS
std_dev = (sum((x - mean) ** 2 for x in simulated_results) / NUM_ITERATIONS)**0.5

print(
    f"Avg: {mean:.0f}\n"
    f"Min: {min(simulated_results)}\n"
    f"Max: {max(simulated_results)}\n"
    f"Std Deviation: {(sum((x - mean)**2 for x in simulated_results) / NUM_ITERATIONS)**0.5:.0f}"
)