import random

NUM_ITERATIONS = 1000
NUM_PRISONERS = 100

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
        
for i in range(0, NUM_ITERATIONS):
    counting_prisoner = random.randint(1, NUM_PRISONERS)
    counted_prisoners = {counting_prisoner}
    num_days_for_escape, counting_prisoner_count = 1, 1
    while counting_prisoner_count < NUM_PRISONERS:
        num_days_for_escape += num_days_for_new_prisoner(counted_prisoners)
        num_days_for_escape += num_days_for_counter(counting_prisoner)
        counting_prisoner_count += 1
    simulated_results.append(num_days_for_escape)

mean = sum(simulated_results) / NUM_ITERATIONS
std_dev = (sum((x - mean) ** 2 for x in simulated_results) / NUM_ITERATIONS)**0.5

print(
    f"Mean: {mean:.0f}\n"
    f"Min: {min(simulated_results)}\n"
    f"Max: {max(simulated_results)}\n"
    f"Mean Standard Error: {std_dev / NUM_ITERATIONS**0.5:.1f}"
)