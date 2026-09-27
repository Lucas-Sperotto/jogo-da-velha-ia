#ifndef GENETIC_H
#define GENETIC_H

#include "learning.h"

/**
 * @brief Agente baseado em Algoritmo Genético (Algoritmos Evolutivos).
 *
 * Uma população de indivíduos (cada um portando um cromossomo de pesos numéricos)
 * evolui ao longo de gerações sob pressão seletiva, elitismo, cruzamento (crossover) e mutação.
 */
typedef struct {
    StrategyWeights best;  /**< Melhor indivíduo (cromossomo de pesos) obtido na evolução. */
    double best_fitness;   /**< Maior pontuação de aptidão (fitness) registrada. */
    int generations;       /**< Total de gerações evoluídas. */
} GeneticAgent;

/**
 * @brief Inicializa o agente genético com valores padrão zerados.
 *
 * @param agent Ponteiro para a estrutura GeneticAgent.
 */
void genetic_init(GeneticAgent *agent);

/**
 * @brief Executa o ciclo evolutivo de treinamento do algoritmo genético.
 *
 * A cada geração:
 *  1. Avalia a aptidão (fitness) de cada indivíduo contra agentes aleatórios e heurísticos;
 *  2. Ordena a população por aptidão decrescente;
 *  3. Preserva a elite (melhores indivíduos) inalterada;
 *  4. Gera novos indivíduos por recombinação aritmética e aplica mutação gaussiana/uniforme.
 *
 * @param agent Ponteiro para o agente genético que armazenará o melhor indivíduo.
 * @param generations Quantidade de gerações a evoluir.
 */
void genetic_train(GeneticAgent *agent, int generations);

/**
 * @brief Salva o estado do agente genético (gerações, melhor fitness e pesos) em arquivo texto.
 *
 * @param agent Ponteiro para o agente.
 * @param path Caminho do arquivo de destino (ex: "data/genetic_weights.dat").
 * @return 1 se salvo com sucesso; 0 caso contrário.
 */
int genetic_save(const GeneticAgent *agent, const char *path);

/**
 * @brief Carrega o estado do agente genético a partir de arquivo texto.
 *
 * @param agent Ponteiro para o agente de destino.
 * @param path Caminho do arquivo a ser lido.
 * @return 1 se lido com sucesso; 0 caso contrário.
 */
int genetic_load(GeneticAgent *agent, const char *path);

/**
 * @brief Função adaptadora de tomada de decisão para a interface MoveSelector.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Ponteiro para a instância GeneticAgent.
 * @return Índice da melhor jogada escolhida pelo melhor indivíduo evoluído.
 */
int agent_genetic_move(Board *board, char player, void *context);

#endif
