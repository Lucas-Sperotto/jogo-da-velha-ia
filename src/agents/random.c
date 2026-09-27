#include "agents.h"

#include <stdlib.h>

/**
 * @brief Seleciona uma jogada aleatória uniforme entre todas as casas livres disponíveis.
 *
 * Atua como baseline estocástico nulo (sem estratégia ou inteligência prévia).
 *
 * @param board Ponteiro para o tabuleiro atual.
 * @param player Símbolo do jogador da vez (não utilizado).
 * @param context Contexto genérico (não utilizado).
 * @return Índice da casa escolhida (0 a 8) ou -1 se não houver jogadas válidas.
 */
int agent_random_move(Board *board, char player, void *context)
{
    int moves[BOARD_SIZE];
    (void)player;
    (void)context;

    int count = board_available_moves(board, moves);
    if (count == 0) {
        return -1;
    }

    return moves[rand() % count];
}
