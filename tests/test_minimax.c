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
        int move=agent_minimax_move(board,ai,NULL);
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

static void test_takes_winning_move(void)
{
    Board b;
    board_init(&b);
    board_make_move(&b,0,PLAYER_O);
    board_make_move(&b,1,PLAYER_O);
    board_make_move(&b,4,PLAYER_X);
    assert(agent_minimax_move(&b,PLAYER_O,NULL) == 2);
}

static void test_minimax_never_loses_as_o(void)
{
    Board b;
    board_init(&b);
    assert(!opponent_can_force_win(&b,PLAYER_X,PLAYER_O,PLAYER_X));
}

static void test_minimax_never_loses_as_x(void)
{
    Board b;
    board_init(&b);
    assert(!opponent_can_force_win(&b,PLAYER_O,PLAYER_X,PLAYER_X));
}

int main(void)
{
    test_takes_winning_move();
    test_minimax_never_loses_as_o();
    test_minimax_never_loses_as_x();
    puts("test_minimax: OK");
    return 0;
}
