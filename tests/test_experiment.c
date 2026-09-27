#include "experiment.h"
#include "rng.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void assert_same_result(const ExperimentResult *a,
                               const ExperimentResult *b)
{
    assert(a->games == b->games);
    assert(a->wins_a == b->wins_a);
    assert(a->wins_b == b->wins_b);
    assert(a->draws == b->draws);
    assert(a->moves == b->moves);
    assert(a->seed == b->seed);
}

static void test_minimax_beats_or_draws_random(void)
{
    RuntimeAgent minimax, random;
    assert(runtime_agent_init(&minimax,AGENT_MINIMAX));
    assert(runtime_agent_init(&random,AGENT_RANDOM));

    ExperimentResult result=run_experiment_seeded(
        &minimax,&random,40,UINT64_C(20260927));

    assert(result.games == 40);
    assert(result.seed == UINT64_C(20260927));
    assert(result.wins_b == 0);
    assert(result.wins_a+result.wins_b+result.draws == 40);

    runtime_agent_destroy(&minimax);
    runtime_agent_destroy(&random);
}

static void test_same_seed_reproduces_experiment(void)
{
    RuntimeAgent a,b;
    assert(runtime_agent_init(&a,AGENT_RANDOM));
    assert(runtime_agent_init(&b,AGENT_RANDOM));

    ExperimentResult first=run_experiment_seeded(
        &a,&b,200,UINT64_C(424242));
    ExperimentResult second=run_experiment_seeded(
        &a,&b,200,UINT64_C(424242));

    assert_same_result(&first,&second);

    runtime_agent_destroy(&a);
    runtime_agent_destroy(&b);
}

static void test_csv_contains_seed(void)
{
    const char *path="tests/test_experiment_output.csv";
    RuntimeAgent a,b;
    char header[256];
    char row[256];

    (void)remove(path);
    assert(runtime_agent_init(&a,AGENT_RANDOM));
    assert(runtime_agent_init(&b,AGENT_RANDOM));

    ExperimentResult result=run_experiment_seeded(
        &a,&b,4,UINT64_C(12345));
    assert(append_experiment_csv(path,&a,&b,&result));

    FILE *file=fopen(path,"r");
    assert(file != NULL);
    assert(fgets(header,sizeof(header),file) != NULL);
    assert(fgets(row,sizeof(row),file) != NULL);
    fclose(file);

    assert(strncmp(header,"seed,",5) == 0);
    assert(strncmp(row,"12345,",6) == 0);

    runtime_agent_destroy(&a);
    runtime_agent_destroy(&b);
    assert(remove(path) == 0);
}

int main(void)
{
    rng_seed(UINT64_C(1));
    test_minimax_beats_or_draws_random();
    test_same_seed_reproduces_experiment();
    test_csv_contains_seed();
    puts("test_experiment: OK");
    return 0;
}
