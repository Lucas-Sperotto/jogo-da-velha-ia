#include "agents.h"

/**
 * @brief Algoritmo recursivo Minimax clássico para busca exaustiva em árvore de jogo.
 *
 * Avaliação de nós folha:
 *  - Vitória da raiz: +(10 - depth) -> prioriza vencer o mais rápido possível;
 *  - Vitória adversária: -(10 - depth) -> prioriza adiar a derrota ao máximo;
 *  - Empate: 0.
 *
 * Nós MAX (turn == root_player) buscam maximizar o valor de retorno.
 * Nós MIN (turn != root_player) buscam minimizar o valor de retorno.
 *
 * @param board Tabuleiro atual sendo explorado na recursão.
 * @param root_player Jogador para o qual a busca está calculando o movimento ótimo.
 * @param turn Jogador que executa a jogada no nível atual da árvore.
 * @param depth Profundidade atual da recursão (número de lances simulados).
 * @param stats Ponteiro para estrutura que acumula a quantidade de nós visitados.
 * @return Pontuação minimax do estado avaliado.
 */
static int minimax(Board *board, char root_player, char turn, int depth, SearchStats *stats)
{
    if (stats != NULL) ++stats->nodes;

    char winner=board_winner(board);
    if (winner == root_player) return 10-depth;
    if (winner == other_player(root_player)) return depth-10;
    if (board_is_full(board)) return 0;

    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);

    if (turn == root_player) {
        int best=-1000;
        for (int i=0;i<count;++i) {
            board_make_move(board,moves[i],turn);
            int score=minimax(board,root_player,other_player(turn),depth+1,stats);
            board_undo_move(board,moves[i]);
            if (score > best) best=score;
        }
        return best;
    }

    int best=1000;
    for (int i=0;i<count;++i) {
        board_make_move(board,moves[i],turn);
        int score=minimax(board,root_player,other_player(turn),depth+1,stats);
        board_undo_move(board,moves[i]);
        if (score < best) best=score;
    }
    return best;
}

/**
 * @brief Ponto de entrada do agente Minimax para seleção da jogada ótima na raiz.
 *
 * Zera as estatísticas de busca, avalia todos os lances imediatos da raiz chamando
 * a função recursiva minimax a partir de depth=1, e retorna o movimento com maior pontuação.
 *
 * @param board Tabuleiro atual.
 * @param player Símbolo do jogador da vez.
 * @param context Ponteiro opcional para SearchStats para coleta de métricas de busca.
 * @return Posição ótima calculada (0 a 8) ou -1 se não houver jogadas disponíveis.
 */
int agent_minimax_move(Board *board, char player, void *context)
{
    SearchStats *stats=context;
    if (stats != NULL) {
        stats->nodes=0;
        stats->prunes=0;
    }

    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    int best_move=-1;
    int best_score=-1000;

    for (int i=0;i<count;++i) {
        board_make_move(board,moves[i],player);
        int score=minimax(board,player,other_player(player),1,stats);
        board_undo_move(board,moves[i]);

        if (score > best_score) {
            best_score=score;
            best_move=moves[i];
        }
    }
    return best_move;
}
