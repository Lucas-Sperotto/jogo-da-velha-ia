#include "learning.h"
#include "rng.h"

/**
 * @brief Conta quantas jogadas imediatas de vitória existem para um determinado jogador.
 *
 * Simula cada jogada disponível e testa se resulta em vitória imediata.
 *
 * @param board Tabuleiro a ser analisado.
 * @param player Jogador avaliado.
 * @return Quantidade de vitórias possíveis em 1 lance (0 a 8).
 */
static int count_immediate_wins(Board *board, char player)
{
    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    int wins=0;
    for (int i=0;i<count;++i) {
        board_make_move(board,moves[i],player);
        if (board_winner(board) == player) ++wins;
        board_undo_move(board,moves[i]);
    }
    return wins;
}

/**
 * @brief Conta quantas peças do jogador estão posicionadas nos quatro cantos (0, 2, 6, 8).
 *
 * @param board Tabuleiro atual.
 * @param player Jogador avaliado.
 * @return Número de cantos controlados (0 a 4).
 */
static int count_corners(const Board *board, char player)
{
    static const int corners[]={0,2,6,8};
    int count=0;
    for (int i=0;i<4;++i) if (board->cells[corners[i]] == player) ++count;
    return count;
}

/**
 * @brief Avalia o potencial de retas abertas (linhas, colunas e diagonais) para o jogador.
 *
 * Para cada uma das 8 retas do tabuleiro:
 *  - Se a reta contiver ao menos uma peça adversária, é considerada bloqueada (score 0);
 *  - Caso contrário, pontua com base no número de peças próprias presentes (own + 1).
 *
 * @param board Tabuleiro atual.
 * @param player Jogador de referência.
 * @return Pontuação cumulativa de potencial de linhas.
 */
static int line_potential(const Board *board, char player)
{
    static const int lines[8][3]={
        {0,1,2},{3,4,5},{6,7,8},{0,3,6},
        {1,4,7},{2,5,8},{0,4,8},{2,4,6}
    };
    char opponent=other_player(player);
    int score=0;
    for (int i=0;i<8;++i) {
        int own=0, blocked=0;
        for (int j=0;j<3;++j) {
            char c=board->cells[lines[i][j]];
            if (c == player) ++own;
            else if (c == opponent) blocked=1;
        }
        if (!blocked) score += own+1;
    }
    return score;
}

/**
 * @brief Conta o total de peças de um jogador no tabuleiro.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador avaliado.
 * @return Número de peças no tabuleiro (0 a 5).
 */
static int count_pieces(const Board *board, char player)
{
    int count=0;
    for (int i=0;i<BOARD_SIZE;++i) if (board->cells[i] == player) ++count;
    return count;
}

/**
 * @brief Extrai o vetor de 6 características (features) contínuas sob a perspectiva do jogador.
 *
 * As 6 dimensões calculadas são:
 *  - out[0]: Termo constante de viés (1.0);
 *  - out[1]: Diferença de vitórias imediatas (player - opponent);
 *  - out[2]: Posse do centro (+1.0 se player, -1.0 se opponent, 0.0 se vazio);
 *  - out[3]: Diferença de cantos ocupados (player - opponent);
 *  - out[4]: Diferença de potencial de linhas abertas (player - opponent);
 *  - out[5]: Diferença de contagem de peças (player - opponent).
 *
 * @param board Tabuleiro atual.
 * @param player Jogador de referência.
 * @param out Vetor de destino com tamanho mínimo FEATURE_COUNT (6).
 */
void extract_features(const Board *board, char player, double out[FEATURE_COUNT])
{
    char opponent=other_player(player);
    Board copy=*board;
    out[0]=1.0;
    out[1]=(double)(count_immediate_wins(&copy,player)-count_immediate_wins(&copy,opponent));
    out[2]=board->cells[4] == player ? 1.0 : board->cells[4] == opponent ? -1.0 : 0.0;
    out[3]=(double)(count_corners(board,player)-count_corners(board,opponent));
    out[4]=(double)(line_potential(board,player)-line_potential(board,opponent));
    out[5]=(double)(count_pieces(board,player)-count_pieces(board,opponent));
}

/**
 * @brief Avalia uma posição calculando o produto escalar entre os pesos e as features extraídas:
 *        Score = Sum_{i=0..5} (weights[i] * features[i])
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da perspectiva de avaliação.
 * @param weights Pesos lineares da estratégia.
 * @return Escalar de avaliação da posição (quanto maior, mais favorável ao player).
 */
double evaluate_position(const Board *board, char player, const StrategyWeights *weights)
{
    double features[FEATURE_COUNT], score=0.0;
    extract_features(board,player,features);
    for (int i=0;i<FEATURE_COUNT;++i) score += weights->values[i]*features[i];
    return score;
}

/**
 * @brief Escolhe a jogada ótima usando busca rasa de 2 níveis combinada com política epsilon-greedy.
 *
 * Funcionamento:
 *  1. Com probabilidade epsilon, escolhe uma jogada aleatória uniforme (exploração);
 *  2. Para cada lance candidato do jogador:
 *     - Se vencer imediatamente, retorna esse lance;
 *     - Simula o lance e examina todas as respostas do adversário, considerando a pior réplica
 *       (se o adversário vencer em resposta, pontua -1000.0, senão avalia a posição resultante);
 *  3. Escolhe o movimento que maximiza a pior réplica adversária (minimax raso de 2 níveis).
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param weights Pesos lineares da função heurística.
 * @param epsilon Probabilidade de exploração aleatória (0.0 para modo totalmente ganancioso).
 * @return Posição escolhida (0 a 8) ou -1 se não houver jogadas válidas.
 */
int weighted_best_move(Board *board, char player, const StrategyWeights *weights, double epsilon)
{
    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    if (count == 0) return -1;

    if (epsilon > 0.0) {
        double r=rng_unit();
        if (r < epsilon) return moves[rng_index((size_t)count)];
    }

    int best_move=moves[0];
    double best_score=-1.0e30;

    for (int i=0;i<count;++i) {
        int move=moves[i];
        board_make_move(board,move,player);

        if (board_winner(board) == player) {
            board_undo_move(board,move);
            return move;
        }

        double score=evaluate_position(board,player,weights);
        int replies[BOARD_SIZE];
        int reply_count=board_available_moves(board,replies);
        double worst_reply=score;

        for (int j=0;j<reply_count;++j) {
            board_make_move(board,replies[j],other_player(player));
            double reply_score=board_winner(board) == other_player(player)
                ? -1000.0 : evaluate_position(board,player,weights);
            board_undo_move(board,replies[j]);
            if (j == 0 || reply_score < worst_reply) worst_reply=reply_score;
        }

        board_undo_move(board,move);
        if (worst_reply > best_score) {
            best_score=worst_reply;
            best_move=move;
        }
    }

    return best_move;
}
