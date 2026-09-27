#ifndef AGENTS_H
#define AGENTS_H

#include "game.h"

typedef struct {
    unsigned long long nodes;
    unsigned long long prunes;
} SearchStats;

int agent_random_move(Board *board, char player, void *context);
int agent_heuristic_move(Board *board, char player, void *context);
int agent_minimax_move(Board *board, char player, void *context);

#endif
