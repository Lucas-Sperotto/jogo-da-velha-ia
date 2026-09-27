#ifndef LEARNING_H
#define LEARNING_H

#include "game.h"

#define FEATURE_COUNT 6

typedef struct {
    double values[FEATURE_COUNT];
} StrategyWeights;

void extract_features(const Board *board, char player, double out[FEATURE_COUNT]);
double evaluate_position(const Board *board, char player, const StrategyWeights *weights);
int weighted_best_move(Board *board, char player, const StrategyWeights *weights, double epsilon);

#endif
