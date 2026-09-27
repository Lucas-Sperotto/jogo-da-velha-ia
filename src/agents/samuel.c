#include "samuel.h"

#include <stdio.h>

/**
 * @brief Registro instantâneo de uma posição observada durante a partida de self-play.
 */
typedef struct {
    double features[FEATURE_COUNT]; /**< Vetor de características extraído do estado. */
    char player;                    /**< Jogador que realizou a ação que levou a este estado. */
} Experience;

/**
 * @brief Inicializa o agente Samuel com pesos heurísticos iniciais e hiperparâmetros padrão.
 *
 * Pesos iniciais:
 *  - viés: 0.0
 *  - vitórias imediatas: 4.0
 *  - centro: 1.5
 *  - cantos: 0.8
 *  - linhas: 0.6
 *  - peças: 0.2
 *
 * @param agent Ponteiro para o agente Samuel.
 */
void samuel_init(SamuelAgent *agent)
{
    static const double initial[FEATURE_COUNT]={0.0,4.0,1.5,0.8,0.6,0.2};
    for (int i=0;i<FEATURE_COUNT;++i) agent->weights.values[i]=initial[i];
    agent->learning_rate=0.01;
    agent->exploration=0.15;
}

/**
 * @brief Seleciona a jogada em modo ganancioso (epsilon=0.0) para a interface MoveSelector.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Ponteiro para a estrutura SamuelAgent.
 * @return Posição escolhida (0 a 8) ou -1 se inválido.
 */
int agent_samuel_move(Board *board, char player, void *context)
{
    SamuelAgent *agent=context;
    if (agent == NULL) return -1;
    return weighted_best_move(board,player,&agent->weights,0.0);
}

/**
 * @brief Treina os pesos da função linear através de auto-jogo (self-play).
 *
 * Processo por episódio:
 *  1. Executa uma partida completa com exploração epsilon = 0.15;
 *  2. Registra o histórico de características de cada estado intermediário;
 *  3. Ao final da partida, define o alvo (Target):
 *     - +10.0 se o jogador do estado venceu a partida;
 *     - -10.0 se o jogador do estado perdeu;
 *     -  0.0 em caso de empate;
 *  4. Calcula o erro (Target - Predição) e atualiza os pesos pelo método do gradiente:
 *     W[i] <- W[i] + learning_rate * error * feature[i].
 *
 * @param agent Ponteiro para o agente a treinar.
 * @param games Quantidade de partidas completas a simular.
 */
void samuel_train(SamuelAgent *agent, int games)
{
    if (agent == NULL || games <= 0) return;

    for (int episode=0;episode<games;++episode) {
        Board board;
        Experience history[BOARD_SIZE];
        int history_count=0;
        char turn=PLAYER_X;
        board_init(&board);

        while (!board_is_terminal(&board)) {
            int move=weighted_best_move(&board,turn,&agent->weights,agent->exploration);
            if (move < 0) break;
            board_make_move(&board,move,turn);

            if (history_count < BOARD_SIZE) {
                extract_features(&board,turn,history[history_count].features);
                history[history_count].player=turn;
                ++history_count;
            }
            turn=other_player(turn);
        }

        char winner=board_winner(&board);
        for (int h=0;h<history_count;++h) {
            double target=0.0;
            if (winner != EMPTY) target=winner == history[h].player ? 10.0 : -10.0;

            double prediction=0.0;
            for (int i=0;i<FEATURE_COUNT;++i)
                prediction += agent->weights.values[i]*history[h].features[i];

            double error=target-prediction;
            for (int i=0;i<FEATURE_COUNT;++i)
                agent->weights.values[i] += agent->learning_rate*error*history[h].features[i];
        }
    }
}

/**
 * @brief Grava os pesos aprendidos em arquivo ASCII com precisão de 17 dígitos significativos.
 *
 * @param agent Agente Samuel cujos pesos serão persistidos.
 * @param path Caminho do arquivo no disco (ex: "data/samuel_weights.dat").
 * @return 1 se bem-sucedido; 0 em caso de falha de abertura/escrita.
 */
int samuel_save(const SamuelAgent *agent, const char *path)
{
    FILE *file=fopen(path,"w");
    if (file == NULL) return 0;
    for (int i=0;i<FEATURE_COUNT;++i)
        fprintf(file,"%.17g%c",agent->weights.values[i],i+1 == FEATURE_COUNT ? '\n' : ' ');
    fclose(file);
    return 1;
}

/**
 * @brief Lê os pesos a partir de um arquivo formatado em disco.
 *
 * @param agent Agente Samuel que receberá os pesos carregados.
 * @param path Caminho do arquivo a ser lido.
 * @return 1 se os FEATURE_COUNT valores foram lidos com sucesso; 0 caso contrário.
 */
int samuel_load(SamuelAgent *agent, const char *path)
{
    StrategyWeights loaded;
    if (agent == NULL || path == NULL) return 0;

    FILE *file=fopen(path,"r");
    if (file == NULL) return 0;

    for (int i=0;i<FEATURE_COUNT;++i) {
        if (fscanf(file,"%lf",&loaded.values[i]) != 1) {
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    agent->weights=loaded;
    return 1;
}
