#include "samuel.h"

#include <assert.h>
#include <stdio.h>

static void test_returns_valid_move(void)
{
    Board b;
    SamuelAgent agent;
    board_init(&b);
    samuel_init(&agent);
    int move=agent_samuel_move(&b,PLAYER_O,&agent);
    assert(board_is_valid_move(&b,move));
}

static void test_training_updates_weights(void)
{
    SamuelAgent agent;
    samuel_init(&agent);
    double before=agent.weights.values[1];
    samuel_train(&agent,50);
    assert(agent.weights.values[1] != before);
}

int main(void)
{
    test_returns_valid_move();
    test_training_updates_weights();
    puts("test_samuel: OK");
    return 0;
}
