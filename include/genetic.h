#ifndef GENETIC_H
#define GENETIC_H

#include "learning.h"

typedef struct {
    StrategyWeights best;
    double best_fitness;
    int generations;
} GeneticAgent;

void genetic_init(GeneticAgent *agent);
void genetic_train(GeneticAgent *agent, int generations);
int genetic_save(const GeneticAgent *agent, const char *path);
int genetic_load(GeneticAgent *agent, const char *path);
int agent_genetic_move(Board *board, char player, void *context);

#endif
