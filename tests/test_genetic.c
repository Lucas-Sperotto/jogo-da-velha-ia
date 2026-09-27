#include "genetic.h"
#include "rng.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void test_truncated_load_is_atomic(void)
{
    const char *path="tests/tmp_genetic_bad.dat";
    GeneticAgent agent;
    GeneticAgent before;

    genetic_init(&agent);
    agent.best.values[0]=3.5;
    agent.best_fitness=7.0;
    agent.generations=4;
    before=agent;

    FILE *file=fopen(path,"w");
    assert(file != NULL);
    fputs("999 1e30\n1.0 2.0\n",file);
    fclose(file);

    assert(!genetic_load(&agent,path));
    assert(memcmp(&agent,&before,sizeof(agent)) == 0);
    assert(remove(path) == 0);
}

int main(void)
{
    GeneticAgent agent;
    Board board;
    rng_seed(UINT64_C(2002));
    genetic_init(&agent);
    genetic_train(&agent,2);
    board_init(&board);

    assert(agent.generations == 2);
    assert(agent.best_fitness > -1.0e20);
    assert(board_is_valid_move(&board,agent_genetic_move(&board,PLAYER_X,&agent)));

    test_truncated_load_is_atomic();

    puts("test_genetic: OK");
    return 0;
}
