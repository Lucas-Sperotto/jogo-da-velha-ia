#ifndef AGENTS_H
#define AGENTS_H

#include "game.h"

/**
 * @brief Estatísticas de busca adversarial coletadas durante uma tomada de decisão.
 */
typedef struct {
    unsigned long long nodes;   /**< Quantidade de nós (estados) visitados na árvore de busca. */
    unsigned long long prunes;  /**< Quantidade de ramos podados (aplicável ao Alpha-Beta). */
} SearchStats;

/**
 * @brief Agente Aleatório (Baseline).
 *
 * Seleciona uniformemente ao acaso uma das casas livres disponíveis.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Não utilizado (NULL).
 * @return Índice da jogada ou -1 se não houver jogadas disponíveis.
 */
int agent_random_move(Board *board, char player, void *context);

/**
 * @brief Agente Heurístico baseado em regras explícitas.
 *
 * Ordem de prioridade das regras:
 *  1. Vencer imediatamente se houver jogada vencedora;
 *  2. Bloquear vitória imediata do adversário;
 *  3. Ocupar o centro (casa 4);
 *  4. Ocupar um dos cantos (0, 2, 6, 8);
 *  5. Escolher a primeira casa livre.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Não utilizado (NULL).
 * @return Índice da jogada ou -1 se não houver jogadas disponíveis.
 */
int agent_heuristic_move(Board *board, char player, void *context);

/**
 * @brief Agente Minimax com busca exaustiva adversarial.
 *
 * Explora a árvore completa do jogo até estados terminais, garantindo jogo ótimo.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Ponteiro opcional para SearchStats para acumular métricas de nós visitados.
 * @return Índice da jogada ótima ou -1 se não houver jogadas disponíveis.
 */
int agent_minimax_move(Board *board, char player, void *context);

/**
 * @brief Agente Minimax otimizado com Poda Alpha-Beta.
 *
 * Produz a mesma decisão ótima do Minimax puro, descartando ramos irrelevantes
 * (onde alpha >= beta), reduzindo drasticamente o número de nós explorados.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Ponteiro opcional para SearchStats para coletar nós e podas.
 * @return Índice da jogada ótima ou -1 se não houver jogadas disponíveis.
 */
int agent_alphabeta_move(Board *board, char player, void *context);

#endif
