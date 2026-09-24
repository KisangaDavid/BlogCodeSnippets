import random

NUM_ITERATIONS = 10000
NUM_PRISONERS = 100

def num_days_for_new_prisoner(counted_prisoners):
    num_days = 1
    while True:
        chosen_prisoner = random.randrange(NUM_PRISONERS)
        if chosen_prisoner not in counted_prisoners:
            counted_prisoners.add(chosen_prisoner)
            return num_days 
        num_days += 1

def num_days_for_counter(counting_prisoner):
    num_days = 1
    while True:
        chosen_prisoner = random.randrange(NUM_PRISONERS)
        if chosen_prisoner == counting_prisoner:
            return num_days
        num_days += 1

sim_results = []
    
for i in range(0, NUM_ITERATIONS):
    counting_prisoner = random.randrange(NUM_PRISONERS)
    counted_prisoners = {counting_prisoner}
    num_days_for_escape, counting_prisoner_count = 1, 1
    while counting_prisoner_count < NUM_PRISONERS:
        num_days_for_escape += num_days_for_new_prisoner(counted_prisoners)
        num_days_for_escape += num_days_for_counter(counting_prisoner)
        counting_prisoner_count += 1
    sim_results.append(num_days_for_escape)

mean = sum(sim_results) / NUM_ITERATIONS
std_dev = (sum((x - mean) ** 2 for x in sim_results) / (NUM_ITERATIONS - 1))**0.5

print(
    f"Mean: {mean:.0f}\n"
    f"Min: {min(sim_results)}\n"
    f"Max: {max(sim_results)}\n"
    f"Standard Error of Mean: {std_dev / NUM_ITERATIONS**0.5:.1f}"
)