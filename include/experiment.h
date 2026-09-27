#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "registry.h"

typedef struct {
    int games;
    int wins_a;
    int wins_b;
    int draws;
    unsigned long long moves;
} ExperimentResult;

char play_ai_match(RuntimeAgent *x, RuntimeAgent *o, int visual);
ExperimentResult run_experiment(RuntimeAgent *a, RuntimeAgent *b, int games);
void print_experiment_result(const RuntimeAgent *a, const RuntimeAgent *b,
                             const ExperimentResult *result);
int append_experiment_csv(const char *path, const RuntimeAgent *a,
                          const RuntimeAgent *b, const ExperimentResult *result);

#endif
