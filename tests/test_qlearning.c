#include "qlearning.h"
#include "rng.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    QLearningAgent agent;
    Board board;

    rng_seed(UINT64_C(3003));
    assert(qlearning_init(&agent));
    qlearning_train(&agent,1000);
    assert(agent.episodes == 1000);

    board_init(&board);
    int move=agent_qlearning_move(&board,PLAYER_X,&agent);
    assert(board_is_valid_move(&board,move));

    qlearning_free(&agent);
    puts("test_qlearning: OK");
    return 0;
}
