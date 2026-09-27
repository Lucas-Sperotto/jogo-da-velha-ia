#include "agents.h"

#include <stdlib.h>

int agent_random_move(Board *board, char player, void *context)
{
    int moves[BOARD_SIZE];
    (void)player;
    (void)context;

    int count = board_available_moves(board, moves);
    if (count == 0) {
        return -1;
    }

    return moves[rand() % count];
}
