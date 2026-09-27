#include "agents.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static int opponent_can_force_win(Board *board, char opponent, char ai, char turn)
{
    char winner=board_winner(board);
    if (winner == opponent) return 1;
    if (winner == ai || board_is_full(board)) return 0;

    if (turn == ai) {
        Board before=*board;
        int move=agent_alphabeta_move(board,ai,NULL);
        assert(memcmp(board,&before,sizeof(before)) == 0);
        assert(move >= 0);
        assert(board_is_valid_move(board,move));

        assert(board_make_move(board,move,ai));
        int result=opponent_can_force_win(board,opponent,ai,opponent);
        board_undo_move(board,move);
        assert(memcmp(board,&before,sizeof(before)) == 0);
        return result;
    }

    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    for (int i=0;i<count;++i) {
        Board before=*board;
        assert(board_make_move(board,moves[i],opponent));
        int result=opponent_can_force_win(board,opponent,ai,ai);
        board_undo_move(board,moves[i]);
        assert(memcmp(board,&before,sizeof(before)) == 0);
        if (result) return 1;
    }
    return 0;
}

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
    assert(memcmp(&b,&copy,sizeof(b)) == 0);
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

static void test_alphabeta_never_loses_as_o(void)
{
    Board b;
    board_init(&b);
    assert(!opponent_can_force_win(&b,PLAYER_X,PLAYER_O,PLAYER_X));
}

static void test_alphabeta_never_loses_as_x(void)
{
    Board b;
    board_init(&b);
    assert(!opponent_can_force_win(&b,PLAYER_O,PLAYER_X,PLAYER_X));
}

int main(void)
{
    test_same_decision_as_minimax();
    test_prunes_search_tree();
    test_alphabeta_never_loses_as_o();
    test_alphabeta_never_loses_as_x();
    puts("test_alphabeta: OK");
    return 0;
}
