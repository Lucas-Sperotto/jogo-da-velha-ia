#ifndef SAMUEL_H
#define SAMUEL_H

#include "learning.h"

typedef struct {
    StrategyWeights weights;
    double learning_rate;
    double exploration;
} SamuelAgent;

void samuel_init(SamuelAgent *agent);
void samuel_train(SamuelAgent *agent, int games);
int samuel_save(const SamuelAgent *agent, const char *path);
int samuel_load(SamuelAgent *agent, const char *path);
int agent_samuel_move(Board *board, char player, void *context);

#endif
