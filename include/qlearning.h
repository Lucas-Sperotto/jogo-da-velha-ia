#ifndef QLEARNING_H
#define QLEARNING_H

#include "game.h"

#define Q_STATE_COUNT 19683
#define Q_ACTION_COUNT 9

typedef struct {
    double *q;
    double alpha;
    double gamma;
    double epsilon;
    int episodes;
} QLearningAgent;

int qlearning_init(QLearningAgent *agent);
void qlearning_free(QLearningAgent *agent);
void qlearning_train(QLearningAgent *agent, int episodes);
int qlearning_save(const QLearningAgent *agent, const char *path);
int qlearning_load(QLearningAgent *agent, const char *path);
int agent_qlearning_move(Board *board, char player, void *context);

#endif
