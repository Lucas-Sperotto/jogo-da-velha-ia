#include "agents.h"

/**
 * @brief Algoritmo recursivo Minimax com Poda Alpha-Beta (Alpha-Beta Pruning).
 *
 * Mantém os limites:
 *  - alpha: a melhor pontuação que o maximizador (root_player) pode garantir até o momento;
 *  - beta: a melhor pontuação que o minimizador (adversário) pode garantir até o momento.
 *
 * Sempre que alpha >= beta, o ramo corrente é podado pois não alterará a decisão final,
 * incrementando stats->prunes. A cada chamada recursiva, stats->nodes é incrementado.
 *
 * @param board Tabuleiro atual sendo percorrido na árvore de busca.
 * @param root_player Jogador para o qual a busca está calculando a jogada ótima.
 * @param turn Jogador com a vez de jogar no nível atual.
 * @param depth Profundidade atual na árvore de busca.
 * @param alpha Limite inferior de pontuação garantido pelo maximizador.
 * @param beta Limite superior de pontuação garantido pelo minimizador.
 * @param stats Estrutura opcional para acumular métricas de nós e podas.
 * @return Pontuação minimax do estado avaliado.
 */
static int alphabeta(Board *board, char root_player, char turn, int depth,
                     int alpha, int beta, SearchStats *stats)
{
    if (stats != NULL) ++stats->nodes;

    char winner=board_winner(board);
    if (winner == root_player) return 10-depth;
    if (winner == other_player(root_player)) return depth-10;
    if (board_is_full(board)) return 0;

    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);

    if (turn == root_player) {
        int value=-1000;
        for (int i=0;i<count;++i) {
            board_make_move(board,moves[i],turn);
            int score=alphabeta(board,root_player,other_player(turn),depth+1,alpha,beta,stats);
            board_undo_move(board,moves[i]);
            if (score > value) value=score;
            if (value > alpha) alpha=value;
            if (alpha >= beta) {
                if (stats != NULL) ++stats->prunes;
                break;
            }
        }
        return value;
    }

    int value=1000;
    for (int i=0;i<count;++i) {
        board_make_move(board,moves[i],turn);
        int score=alphabeta(board,root_player,other_player(turn),depth+1,alpha,beta,stats);
        board_undo_move(board,moves[i]);
        if (score < value) value=score;
        if (value < beta) beta=value;
        if (alpha >= beta) {
            if (stats != NULL) ++stats->prunes;
            break;
        }
    }
    return value;
}

/**
 * @brief Ponto de entrada do agente Alpha-Beta para decisão ótima na raiz.
 *
 * Inicia os limites com alpha = -1000 e beta = +1000, atualizando o valor de alpha
 * à medida que melhores opções são encontradas e propagando o corte para ramos filhos.
 *
 * @param board Tabuleiro atual.
 * @param player Símbolo do jogador da vez.
 * @param context Ponteiro opcional para SearchStats.
 * @return Índice do melhor movimento (0 a 8) ou -1 se não houver jogadas disponíveis.
 */
int agent_alphabeta_move(Board *board, char player, void *context)
{
    SearchStats *stats=context;
    if (stats != NULL) {
        stats->nodes=0;
        stats->prunes=0;
    }

    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    int best_move=-1, best_score=-1000, alpha=-1000;
    const int beta=1000;

    for (int i=0;i<count;++i) {
        board_make_move(board,moves[i],player);
        int score=alphabeta(board,player,other_player(player),1,alpha,beta,stats);
        board_undo_move(board,moves[i]);

        if (score > best_score) {
            best_score=score;
            best_move=moves[i];
        }
        if (best_score > alpha) alpha=best_score;
    }

    return best_move;
}
