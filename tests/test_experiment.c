#include "experiment.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    RuntimeAgent minimax, random;
    assert(runtime_agent_init(&minimax,AGENT_MINIMAX));
    assert(runtime_agent_init(&random,AGENT_RANDOM));

    ExperimentResult result=run_experiment(&minimax,&random,40);
    assert(result.games == 40);
    assert(result.wins_b == 0);
    assert(result.wins_a+result.wins_b+result.draws == 40);

    runtime_agent_destroy(&minimax);
    runtime_agent_destroy(&random);
    puts("test_experiment: OK");
    return 0;
}
