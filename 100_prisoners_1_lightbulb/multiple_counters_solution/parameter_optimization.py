from types import SimpleNamespace
from sim import estimate_mean
from skopt import gp_minimize
from skopt.space import Integer, Categorical

NUM_ITERATIONS = 750

search_space = [
    Categorical([5, 10, 20], name="num_sub_counters"),
    Integer(250, 2500, name="stage_1_length"),
    Integer(250, 2500, name="stage_2_length"),
    Integer(50, 500, name="secondary_stage_1_length"),
    Integer(50, 500, name="secondary_stage_2_length"),
]

def build_config(params):
    (
        num_sub_counters, 
        stage_1_length, 
        stage_2_length, 
        secondary_stage_1_length, 
        secondary_stage_2_length
     ) = params
    return SimpleNamespace(
        num_iterations=NUM_ITERATIONS,
        num_prisoners=100,
        num_sub_counters=num_sub_counters,
        stage_1_length=stage_1_length,
        stage_2_length=stage_2_length,
        secondary_stage_1_length=secondary_stage_1_length,
        secondary_stage_2_length=secondary_stage_2_length,
    )

def objective(params):
    config = build_config(params)
    return estimate_mean(config)

result = gp_minimize(
    func=objective,
    dimensions=search_space,
    n_calls=10,
    n_initial_points=10,
    random_state=48,
)

print(
    f"Number of sub-counters: {result.x[0]}\n"
    f"Stage 1 length:   {result.x[1]}\n"
    f"Stage 2 length:   {result.x[2]} \n"
    f"Subsequent stage 1 length:   {result.x[3]}\n"
    f"Subsequent stage 2 length:   {result.x[4]}\n"
)