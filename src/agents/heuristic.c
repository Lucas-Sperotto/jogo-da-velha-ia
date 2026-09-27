#include "agents.h"

/**
 * @brief Varre as jogadas disponíveis para verificar se alguma gera vitória imediata.
 *
 * Aplica temporariamente cada movimento possível com board_make_move, testa se
 * board_winner retorna o jogador em questão, e reverte com board_undo_move.
 *
 * @param board Ponteiro para o tabuleiro atual.
 * @param player Jogador para o qual busca-se a vitória imediata.
 * @return Índice da jogada vencedora (0 a 8) ou -1 se não houver vitória em 1 lance.
 */
static int find_immediate_win(Board *board, char player)
{
    int moves[BOARD_SIZE];
    int count = board_available_moves(board, moves);

    for (int i = 0; i < count; ++i) {
        int move = moves[i];
        board_make_move(board, move, player);
        char winner = board_winner(board);
        board_undo_move(board, move);

        if (winner == player) {
            return move;
        }
    }

    return -1;
}

/**
 * @brief Seleciona a jogada com base em uma árvore de decisão de regras prioritárias.
 *
 * Ordem de prioridade estrita:
 *  1. Vitória imediata da própria IA;
 *  2. Bloqueio de vitória iminente do adversário;
 *  3. Ocupação da casa central (posição 4);
 *  4. Ocupação de um dos cantos (0, 2, 6, 8);
 *  5. Escolha da primeira posição livre restante.
 *
 * @param board Ponteiro para o tabuleiro atual.
 * @param player Símbolo do jogador da vez.
 * @param context Contexto genérico (não utilizado).
 * @return Posição escolhida (0 a 8) ou -1 se o tabuleiro estiver cheio.
 */
int agent_heuristic_move(Board *board, char player, void *context)
{
    static const int corners[] = {0, 2, 6, 8};
    int moves[BOARD_SIZE];
    (void)context;

    int move = find_immediate_win(board, player);
    if (move >= 0) return move;

    move = find_immediate_win(board, other_player(player));
    if (move >= 0) return move;

    if (board_is_valid_move(board, 4)) return 4;

    for (int i = 0; i < 4; ++i) {
        if (board_is_valid_move(board, corners[i])) return corners[i];
    }

    int count = board_available_moves(board, moves);
    return count > 0 ? moves[0] : -1;
}
