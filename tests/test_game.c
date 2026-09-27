#include "game.h"

#include <assert.h>
#include <stdio.h>

static void test_initial_board(void)
{
    Board board;
    board_init(&board);
    assert(board_available_moves(&board,NULL) == 9);
    assert(board_winner(&board) == EMPTY);
    assert(!board_is_terminal(&board));
}

static void test_valid_moves(void)
{
    Board board;
    board_init(&board);
    assert(board_make_move(&board,4,PLAYER_X));
    assert(!board_make_move(&board,4,PLAYER_O));
    assert(board.cells[4] == PLAYER_X);
    board_undo_move(&board,4);
    assert(board.cells[4] == EMPTY);
}

static void test_winner(void)
{
    Board board;
    board_init(&board);
    board_make_move(&board,0,PLAYER_X);
    board_make_move(&board,4,PLAYER_X);
    board_make_move(&board,8,PLAYER_X);
    assert(board_winner(&board) == PLAYER_X);
    assert(board_is_terminal(&board));
}

int main(void)
{
    test_initial_board();
    test_valid_moves();
    test_winner();
    puts("test_game: OK");
    return 0;
}
