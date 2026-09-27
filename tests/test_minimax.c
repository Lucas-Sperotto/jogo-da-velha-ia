#include "agents.h"

#include <assert.h>
#include <stdio.h>

static int human_can_force_win(Board *board, char human, char ai, char turn)
{
    char winner=board_winner(board);
    if (winner == human) return 1;
    if (winner == ai || board_is_full(board)) return 0;

    if (turn == ai) {
        int move=agent_minimax_move(board,ai,NULL);
        assert(move >= 0);
        board_make_move(board,move,ai);
        int result=human_can_force_win(board,human,ai,human);
        board_undo_move(board,move);
        return result;
    }

    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    for (int i=0;i<count;++i) {
        board_make_move(board,moves[i],human);
        int result=human_can_force_win(board,human,ai,ai);
        board_undo_move(board,moves[i]);
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
    assert(!human_can_force_win(&b,PLAYER_X,PLAYER_O,PLAYER_X));
}

int main(void)
{
    test_takes_winning_move();
    test_minimax_never_loses_as_o();
    puts("test_minimax: OK");
    return 0;
}
