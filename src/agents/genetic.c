#include "genetic.h"
#include "rng.h"
#include "agents.h"

#include <stdio.h>
#include <stdlib.h>

#define POPULATION_SIZE 32
#define ELITE_COUNT 4
#define EVAL_GAMES 12

/**
 * @brief Representação de um indivíduo da população genética.
 */
typedef struct {
    StrategyWeights weights; /**< Cromossomo: vetor de 6 pesos lineares da estratégia. */
    double fitness;          /**< Pontuação de aptidão acumulada nos jogos de avaliação. */
} Individual;

/**
 * @brief Gera um número pseudoaleatório no ponto flutuante dentro do intervalo [min, max].
 *
 * @param min Limite inferior.
 * @param max Limite superior.
 * @return Valor aleatório no intervalo [min, max].
 */
static double random_between(double min, double max)
{
    return min + (max - min) * rng_unit();
}

/**
 * @brief Executa uma partida de avaliação entre um conjunto de pesos e um oponente especificado.
 *
 * @param weights Pesos do indivíduo avaliado.
 * @param evolved_player Símbolo atribuído ao indivíduo ('X' ou 'O').
 * @param opponent Função MoveSelector do adversário (heurístico ou aleatório).
 * @return +1 se o indivíduo venceu, 0 em empate, -1 se perdeu.
 */
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

/**
 * @brief Calcula a aptidão (fitness) total de um indivíduo através de EVAL_GAMES partidas.
 *
 * O indivíduo joga metade das partidas como 'X' e metade como 'O', enfrentando
 * adversários aleatórios e heurísticos.
 * Pontuação:
 *  - Vitória: +3.0
 *  - Empate: +1.0
 *  - Derrota: -2.0
 *
 * @param weights Pesos do indivíduo.
 * @return Aptidão total acumulada.
 */
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

/**
 * @brief Função comparadora para ordenação decrescente de indivíduos por aptidão (fitness) via qsort.
 *
 * @param a Ponteiro para o primeiro indivíduo.
 * @param b Ponteiro para o segundo indivíduo.
 * @return -1 se a tem maior fitness que b, 1 se menor, 0 se igual.
 */
static int compare_individuals(const void *a, const void *b)
{
    const Individual *ia=a, *ib=b;
    if (ia->fitness < ib->fitness) return 1;
    if (ia->fitness > ib->fitness) return -1;
    return 0;
}

/**
 * @brief Operador de cruzamento aritmético (crossover contínuo) entre dois indivíduos pais.
 *
 * Para cada peso i: Child[i] = mix * ParentA[i] + (1 - mix) * ParentB[i], com mix ~ U(0, 1).
 *
 * @param a Estrutura de pesos do primeiro progenitor.
 * @param b Estrutura de pesos do segundo progenitor.
 * @return Novo indivíduo filho recombinado.
 */
static StrategyWeights crossover(const StrategyWeights *a, const StrategyWeights *b)
{
    StrategyWeights child;
    for (int i=0;i<FEATURE_COUNT;++i) {
        double mix=random_between(0.0,1.0);
        child.values[i]=mix*a->values[i]+(1.0-mix)*b->values[i];
    }
    return child;
}

/**
 * @brief Operador de mutação gênica: com 25% de probabilidade por peso, adiciona ruído em [-1.0, 1.0].
 *
 * @param weights Ponteiro para o cromossomo a ser mutado.
 */
static void mutate(StrategyWeights *weights)
{
    for (int i=0;i<FEATURE_COUNT;++i) {
        if (random_between(0.0,1.0) < 0.25)
            weights->values[i] += random_between(-1.0,1.0);
    }
}

/**
 * @brief Inicializa a estrutura do agente genético com valores padrão.
 *
 * @param agent Ponteiro para a estrutura GeneticAgent.
 */
void genetic_init(GeneticAgent *agent)
{
    for (int i=0;i<FEATURE_COUNT;++i) agent->best.values[i]=0.0;
    agent->best_fitness=-1.0e30;
    agent->generations=0;
}

/**
 * @brief Executa o ciclo evolutivo ao longo do número de gerações especificado.
 *
 * Passos por geração:
 *  1. Avalia o fitness de todos os indivíduos da população;
 *  2. Ordena a população por fitness decrescente;
 *  3. Atualiza o melhor indivíduo histórico se houver recorde de fitness;
 *  4. Preserva os ELITE_COUNT (4) melhores indivíduos diretamente para a próxima geração;
 *  5. Preenche as vagas restantes através de seleção nos top 50%, crossover e mutação.
 *
 * @param agent Ponteiro para o agente genético.
 * @param generations Número de gerações a evoluir.
 */
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
            int a=(int)rng_index(POPULATION_SIZE/2);
            int b=(int)rng_index(POPULATION_SIZE/2);
            next[p].weights=crossover(&population[a].weights,&population[b].weights);
            mutate(&next[p].weights);
            next[p].fitness=0.0;
        }

        for (int p=0;p<POPULATION_SIZE;++p) population[p]=next[p];
        ++agent->generations;
    }
}

/**
 * @brief Seleciona a jogada em modo ganancioso (epsilon=0.0) usando o melhor indivíduo evoluído.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Ponteiro para a instância GeneticAgent.
 * @return Posição escolhida (0 a 8) ou -1 se inválido.
 */
int agent_genetic_move(Board *board, char player, void *context)
{
    GeneticAgent *agent=context;
    if (agent == NULL) return -1;
    return weighted_best_move(board,player,&agent->best,0.0);
}

/**
 * @brief Salva o estado do agente genético (gerações, melhor fitness e pesos) em arquivo ASCII.
 *
 * @param agent Agente genético a ser salvo.
 * @param path Caminho do arquivo no disco (ex: "data/genetic_weights.dat").
 * @return 1 se salvo com sucesso; 0 em caso de erro de abertura/escrita.
 */
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

/**
 * @brief Carrega o estado do agente genético a partir de arquivo de texto.
 *
 * @param agent Agente genético de destino.
 * @param path Caminho do arquivo a ser lido.
 * @return 1 se lido com sucesso; 0 caso contrário.
 */
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
