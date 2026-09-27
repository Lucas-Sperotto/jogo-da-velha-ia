#include "agents.h"

#include <assert.h>
#include <stdio.h>

static void test_heuristic_wins(void)
{
    Board b;
    board_init(&b);
    board_make_move(&b,0,PLAYER_O);
    board_make_move(&b,1,PLAYER_O);
    assert(agent_heuristic_move(&b,PLAYER_O,NULL) == 2);
}

static void test_heuristic_blocks(void)
{
    Board b;
    board_init(&b);
    board_make_move(&b,3,PLAYER_X);
    board_make_move(&b,4,PLAYER_X);
    assert(agent_heuristic_move(&b,PLAYER_O,NULL) == 5);
}

static void test_heuristic_takes_center(void)
{
    Board b;
    board_init(&b);
    assert(agent_heuristic_move(&b,PLAYER_O,NULL) == 4);
}

int main(void)
{
    test_heuristic_wins();
    test_heuristic_blocks();
    test_heuristic_takes_center();
    puts("test_agents: OK");
    return 0;
}
