#include "samuel.h"

#include <stdio.h>

typedef struct {
    double features[FEATURE_COUNT];
    char player;
} Experience;

void samuel_init(SamuelAgent *agent)
{
    static const double initial[FEATURE_COUNT]={0.0,4.0,1.5,0.8,0.6,0.2};
    for (int i=0;i<FEATURE_COUNT;++i) agent->weights.values[i]=initial[i];
    agent->learning_rate=0.01;
    agent->exploration=0.15;
}

int agent_samuel_move(Board *board, char player, void *context)
{
    SamuelAgent *agent=context;
    if (agent == NULL) return -1;
    return weighted_best_move(board,player,&agent->weights,0.0);
}

void samuel_train(SamuelAgent *agent, int games)
{
    if (agent == NULL || games <= 0) return;

    for (int episode=0;episode<games;++episode) {
        Board board;
        Experience history[BOARD_SIZE];
        int history_count=0;
        char turn=PLAYER_X;
        board_init(&board);

        while (!board_is_terminal(&board)) {
            int move=weighted_best_move(&board,turn,&agent->weights,agent->exploration);
            if (move < 0) break;
            board_make_move(&board,move,turn);

            if (history_count < BOARD_SIZE) {
                extract_features(&board,turn,history[history_count].features);
                history[history_count].player=turn;
                ++history_count;
            }
            turn=other_player(turn);
        }

        char winner=board_winner(&board);
        for (int h=0;h<history_count;++h) {
            double target=0.0;
            if (winner != EMPTY) target=winner == history[h].player ? 10.0 : -10.0;

            double prediction=0.0;
            for (int i=0;i<FEATURE_COUNT;++i)
                prediction += agent->weights.values[i]*history[h].features[i];

            double error=target-prediction;
            for (int i=0;i<FEATURE_COUNT;++i)
                agent->weights.values[i] += agent->learning_rate*error*history[h].features[i];
        }
    }
}

int samuel_save(const SamuelAgent *agent, const char *path)
{
    FILE *file=fopen(path,"w");
    if (file == NULL) return 0;
    for (int i=0;i<FEATURE_COUNT;++i)
        fprintf(file,"%.17g%c",agent->weights.values[i],i+1 == FEATURE_COUNT ? '\n' : ' ');
    fclose(file);
    return 1;
}

int samuel_load(SamuelAgent *agent, const char *path)
{
    FILE *file=fopen(path,"r");
    if (file == NULL) return 0;
    for (int i=0;i<FEATURE_COUNT;++i) {
        if (fscanf(file,"%lf",&agent->weights.values[i]) != 1) {
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return 1;
}
