#include "agents.h"

static int find_immediate_win(Board *board, char player)
{
    int moves[BOARD_SIZE];
    int count = board_available_moves(board, moves);

    for (int i = 0; i < count; ++i) {
        int move = moves[i];
        board_make_move(board, move, player);
        char winner = board_winner(board);
        board_undo_move(board, move);

        if (winner == player) {
            return move;
        }
    }

    return -1;
}

int agent_heuristic_move(Board *board, char player, void *context)
{
    static const int corners[] = {0, 2, 6, 8};
    int moves[BOARD_SIZE];
    (void)context;

    int move = find_immediate_win(board, player);
    if (move >= 0) return move;

    move = find_immediate_win(board, other_player(player));
    if (move >= 0) return move;

    if (board_is_valid_move(board, 4)) return 4;

    for (int i = 0; i < 4; ++i) {
        if (board_is_valid_move(board, corners[i])) return corners[i];
    }

    int count = board_available_moves(board, moves);
    return count > 0 ? moves[0] : -1;
}
