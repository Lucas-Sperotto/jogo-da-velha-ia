#include "genetic.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    GeneticAgent agent;
    Board board;
    genetic_init(&agent);
    genetic_train(&agent,2);
    board_init(&board);

    assert(agent.generations == 2);
    assert(agent.best_fitness > -1.0e20);
    assert(board_is_valid_move(&board,agent_genetic_move(&board,PLAYER_X,&agent)));

    puts("test_genetic: OK");
    return 0;
}
