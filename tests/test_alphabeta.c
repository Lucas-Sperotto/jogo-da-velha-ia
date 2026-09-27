#include "agents.h"

#include <assert.h>
#include <stdio.h>

static void test_same_decision_as_minimax(void)
{
    Board b;
    board_init(&b);
    board_make_move(&b,4,PLAYER_X);
    board_make_move(&b,0,PLAYER_O);
    board_make_move(&b,8,PLAYER_X);

    Board copy=b;
    assert(agent_minimax_move(&b,PLAYER_O,NULL) ==
           agent_alphabeta_move(&copy,PLAYER_O,NULL));
}

static void test_prunes_search_tree(void)
{
    Board a,b;
    SearchStats minimax_stats={0};
    SearchStats alpha_stats={0};
    board_init(&a);
    board_init(&b);

    (void)agent_minimax_move(&a,PLAYER_X,&minimax_stats);
    (void)agent_alphabeta_move(&b,PLAYER_X,&alpha_stats);

    assert(alpha_stats.prunes > 0);
    assert(alpha_stats.nodes < minimax_stats.nodes);
}

int main(void)
{
    test_same_decision_as_minimax();
    test_prunes_search_tree();
    puts("test_alphabeta: OK");
    return 0;
}
