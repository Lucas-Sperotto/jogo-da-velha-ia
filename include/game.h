#ifndef GAME_H
#define GAME_H

#include <stddef.h>

#define BOARD_SIZE 9
#define EMPTY ' '
#define PLAYER_X 'X'
#define PLAYER_O 'O'

typedef struct {
    char cells[BOARD_SIZE];
} Board;

void board_init(Board *board);
void board_print(const Board *board);
int board_is_valid_move(const Board *board, int move);
int board_make_move(Board *board, int move, char player);
void board_undo_move(Board *board, int move);
char board_winner(const Board *board);
int board_is_full(const Board *board);
int board_is_terminal(const Board *board);
int board_available_moves(const Board *board, int moves[BOARD_SIZE]);
char other_player(char player);
int read_human_move(const Board *board, char player);
void clear_screen(void);
void wait_enter(void);
void play_human_vs_human(void);

#endif
