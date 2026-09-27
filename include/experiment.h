#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "registry.h"

/**
 * @brief Estrutura que agrega as métricas resultantes de uma série de partidas entre dois agentes.
 */
typedef struct {
    int games;                 /**< Total de partidas disputadas no experimento. */
    int wins_a;                /**< Número de vitórias do Agente A. */
    int wins_b;                /**< Número de vitórias do Agente B. */
    int draws;                 /**< Número de partidas finalizadas em empate. */
    unsigned long long moves;  /**< Soma total de jogadas realizadas em todas as partidas. */
} ExperimentResult;

/**
 * @brief Simula uma única partida entre dois agentes de IA (x jogando com 'X', o com 'O').
 *
 * @param x Agente que joga com as peças 'X'.
 * @param o Agente que joga com as peças 'O'.
 * @param visual Se diferente de zero, imprime cada estado do tabuleiro e jogada no console.
 * @return Símbolo do vencedor ('X' ou 'O') ou EMPTY (' ') em caso de empate.
 */
char play_ai_match(RuntimeAgent *x, RuntimeAgent *o, int visual);

/**
 * @brief Executa um experimento estatístico com múltiplas partidas entre dois agentes.
 *
 * Alterna sistematicamente quem inicia as partidas (jogos pares: A é 'X' e B é 'O';
 * jogos ímpares: B é 'X' e A é 'O') para eliminar viés estatístico de primeiro jogador.
 *
 * @param a Primeiro agente (Agente A).
 * @param b Segundo agente (Agente B).
 * @param games Quantidade de partidas a simular.
 * @return Estrutura ExperimentResult preenchida com os resultados agregados.
 */
ExperimentResult run_experiment(RuntimeAgent *a, RuntimeAgent *b, int games);

/**
 * @brief Imprime no console um resumo legível e formatado dos resultados do experimento,
 *        incluindo contagem de vitórias, empates, médias de jogadas e nós/podas.
 *
 * @param a Agente A.
 * @param b Agente B.
 * @param result Ponteiro para a estrutura com os resultados coletados.
 */
void print_experiment_result(const RuntimeAgent *a, const RuntimeAgent *b,
                             const ExperimentResult *result);

/**
 * @brief Registra o resultado do experimento em um arquivo CSV para posterior análise científica.
 *
 * Se o arquivo estiver vazio ou for criado neste momento, adiciona automaticamente a linha de cabeçalho.
 *
 * @param path Caminho do arquivo CSV de destino (ex: "results/experiments.csv").
 * @param a Agente A.
 * @param b Agente B.
 * @param result Ponteiro para o resultado do experimento.
 * @return 1 se os dados foram anexados com sucesso; 0 em caso de erro ao abrir ou gravar no arquivo.
 */
int append_experiment_csv(const char *path, const RuntimeAgent *a,
                          const RuntimeAgent *b, const ExperimentResult *result);

#endif
