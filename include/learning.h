#ifndef LEARNING_H
#define LEARNING_H

#include "game.h"

/**
 * @def FEATURE_COUNT
 * @brief Número de características (features) extraídas do tabuleiro para a função linear.
 *
 * As 6 características são:
 *  - [0]: Termo de viés (bias = 1.0)
 *  - [1]: Diferença de vitórias imediatas (jogador - oponente)
 *  - [2]: Ocupação da casa central (1.0 = jogador, -1.0 = oponente, 0.0 = livre)
 *  - [3]: Diferença de ocupação dos cantos (jogador - oponente)
 *  - [4]: Potencial de linhas abertas não bloqueadas (jogador - oponente)
 *  - [5]: Diferença do número de peças no tabuleiro (jogador - oponente)
 */
#define FEATURE_COUNT 6

/**
 * @brief Vetor de pesos da estratégia para avaliação linear da posição.
 */
typedef struct {
    double values[FEATURE_COUNT]; /**< Coeficientes numéricos para cada feature correspondente. */
} StrategyWeights;

/**
 * @brief Extrai o vetor de características a partir do estado do tabuleiro sob a ótica de um jogador.
 *
 * @param board Ponteiro para o tabuleiro atual.
 * @param player Símbolo do jogador de referência (PLAYER_X ou PLAYER_O).
 * @param out Vetor de saída de tamanho FEATURE_COUNT onde os valores calculados são armazenados.
 */
void extract_features(const Board *board, char player, double out[FEATURE_COUNT]);

/**
 * @brief Avalia a qualidade de uma posição de tabuleiro por combinação linear: Score = Sum(W[i] * F[i]).
 *
 * @param board Ponteiro para o tabuleiro.
 * @param player Símbolo do jogador sob cuja perspectiva a posição é avaliada.
 * @param weights Vetor de pesos da estratégia do agente.
 * @return Valor numérico escalar (score). Valores mais altos indicam maior vantagem para o jogador.
 */
double evaluate_position(const Board *board, char player, const StrategyWeights *weights);

/**
 * @brief Seleciona a melhor jogada usando busca de 2 níveis (1-ply do jogador + 1-ply de resposta adversária)
 *        com política epsilon-greedy para exploração/exploitation.
 *
 * @param board Ponteiro para o tabuleiro atual.
 * @param player Símbolo do jogador da vez.
 * @param weights Pesos da estratégia linear para avaliação das posições.
 * @param epsilon Taxa de exploração aleatória (0.0 = totalmente ganancioso/greedy).
 * @return Índice da jogada escolhida (0 a 8) ou -1 se não houver jogadas disponíveis.
 */
int weighted_best_move(Board *board, char player, const StrategyWeights *weights, double epsilon);

#endif
