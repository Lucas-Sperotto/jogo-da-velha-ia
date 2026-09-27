#include "genetic.h"
#include "agents.h"

#include <stdio.h>
#include <stdlib.h>

#define POPULATION_SIZE 32
#define ELITE_COUNT 4
#define EVAL_GAMES 12

typedef struct {
    StrategyWeights weights;
    double fitness;
} Individual;

static double random_between(double min, double max)
{
    return min + (max - min) * ((double)rand() / (double)RAND_MAX);
}

static int play_match(const StrategyWeights *weights, char evolved_player,
                      MoveSelector opponent)
{
    Board board;
    char turn=PLAYER_X;
    board_init(&board);

    while (!board_is_terminal(&board)) {
        int move=turn == evolved_player
            ? weighted_best_move(&board,turn,weights,0.0)
            : opponent(&board,turn,NULL);
        if (move < 0 || !board_make_move(&board,move,turn))
            return turn == evolved_player ? -1 : 1;
        turn=other_player(turn);
    }

    char winner=board_winner(&board);
    if (winner == EMPTY) return 0;
    return winner == evolved_player ? 1 : -1;
}

static double evaluate_individual(const StrategyWeights *weights)
{
    double fitness=0.0;
    for (int i=0;i<EVAL_GAMES;++i) {
        char side=(i%2 == 0) ? PLAYER_X : PLAYER_O;
        MoveSelector opponent=(i%3 == 0) ? agent_heuristic_move : agent_random_move;
        int result=play_match(weights,side,opponent);
        if (result > 0) fitness += 3.0;
        else if (result == 0) fitness += 1.0;
        else fitness -= 2.0;
    }
    return fitness;
}

static int compare_individuals(const void *a, const void *b)
{
    const Individual *ia=a, *ib=b;
    if (ia->fitness < ib->fitness) return 1;
    if (ia->fitness > ib->fitness) return -1;
    return 0;
}

static StrategyWeights crossover(const StrategyWeights *a, const StrategyWeights *b)
{
    StrategyWeights child;
    for (int i=0;i<FEATURE_COUNT;++i) {
        double mix=random_between(0.0,1.0);
        child.values[i]=mix*a->values[i]+(1.0-mix)*b->values[i];
    }
    return child;
}

static void mutate(StrategyWeights *weights)
{
    for (int i=0;i<FEATURE_COUNT;++i) {
        if (random_between(0.0,1.0) < 0.25)
            weights->values[i] += random_between(-1.0,1.0);
    }
}

void genetic_init(GeneticAgent *agent)
{
    for (int i=0;i<FEATURE_COUNT;++i) agent->best.values[i]=0.0;
    agent->best_fitness=-1.0e30;
    agent->generations=0;
}

void genetic_train(GeneticAgent *agent, int generations)
{
    if (agent == NULL || generations <= 0) return;

    Individual population[POPULATION_SIZE], next[POPULATION_SIZE];
    for (int p=0;p<POPULATION_SIZE;++p) {
        for (int i=0;i<FEATURE_COUNT;++i)
            population[p].weights.values[i]=random_between(-3.0,3.0);
        population[p].fitness=0.0;
    }

    for (int generation=0;generation<generations;++generation) {
        for (int p=0;p<POPULATION_SIZE;++p)
            population[p].fitness=evaluate_individual(&population[p].weights);

        qsort(population,POPULATION_SIZE,sizeof(population[0]),compare_individuals);

        if (population[0].fitness > agent->best_fitness) {
            agent->best=population[0].weights;
            agent->best_fitness=population[0].fitness;
        }

        for (int p=0;p<ELITE_COUNT;++p) next[p]=population[p];

        for (int p=ELITE_COUNT;p<POPULATION_SIZE;++p) {
            int a=rand()%(POPULATION_SIZE/2);
            int b=rand()%(POPULATION_SIZE/2);
            next[p].weights=crossover(&population[a].weights,&population[b].weights);
            mutate(&next[p].weights);
            next[p].fitness=0.0;
        }

        for (int p=0;p<POPULATION_SIZE;++p) population[p]=next[p];
        ++agent->generations;
    }
}

int agent_genetic_move(Board *board, char player, void *context)
{
    GeneticAgent *agent=context;
    if (agent == NULL) return -1;
    return weighted_best_move(board,player,&agent->best,0.0);
}

int genetic_save(const GeneticAgent *agent, const char *path)
{
    FILE *file=fopen(path,"w");
    if (file == NULL) return 0;
    fprintf(file,"%d %.17g\n",agent->generations,agent->best_fitness);
    for (int i=0;i<FEATURE_COUNT;++i)
        fprintf(file,"%.17g%c",agent->best.values[i],i+1 == FEATURE_COUNT ? '\n' : ' ');
    fclose(file);
    return 1;
}

int genetic_load(GeneticAgent *agent, const char *path)
{
    FILE *file=fopen(path,"r");
    if (file == NULL) return 0;
    if (fscanf(file,"%d %lf",&agent->generations,&agent->best_fitness) != 2) {
        fclose(file);
        return 0;
    }
    for (int i=0;i<FEATURE_COUNT;++i) {
        if (fscanf(file,"%lf",&agent->best.values[i]) != 1) {
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    return 1;
}
