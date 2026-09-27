#ifndef SAMUEL_H
#define SAMUEL_H

#include "learning.h"

/**
 * @brief Agente de aprendizado por auto-jogo (self-play) inspirado nos trabalhos pioneiros de Arthur Samuel.
 *
 * Utiliza uma função linear de avaliação sobre características do jogo e atualiza seus pesos
 * através do método do gradiente descendente após o término de cada partida.
 */
typedef struct {
    StrategyWeights weights; /**< Coeficientes lineares atuais da função de utilidade. */
    double learning_rate;    /**< Taxa de aprendizado (passo do gradiente, ex: 0.01). */
    double exploration;      /**< Probabilidade de escolher uma jogada aleatória durante treino (epsilon). */
} SamuelAgent;

/**
 * @brief Inicializa o agente Samuel com hiperparâmetros padrão e pesos heurísticos iniciais.
 *
 * @param agent Ponteiro para a estrutura SamuelAgent.
 */
void samuel_init(SamuelAgent *agent);

/**
 * @brief Treina o agente Samuel executando partidas de auto-jogo (self-play).
 *
 * Ao final de cada partida, calcula o erro entre o resultado obtido e a previsão feita
 * durante o jogo, ajustando os pesos na direção do resultado real.
 *
 * @param agent Ponteiro para o agente a ser treinado.
 * @param games Quantidade de partidas completas de auto-jogo a serem simuladas.
 */
void samuel_train(SamuelAgent *agent, int games);

/**
 * @brief Salva os pesos aprendidos pelo agente em arquivo de texto formatado.
 *
 * @param agent Ponteiro para o agente.
 * @param path Caminho do arquivo de destino (ex: "data/samuel_weights.dat").
 * @return 1 se o salvamento foi concluído com sucesso; 0 caso ocorra erro de E/S.
 */
int samuel_save(const SamuelAgent *agent, const char *path);

/**
 * @brief Carrega os pesos de um arquivo de texto formatado para o agente.
 *
 * @param agent Ponteiro para o agente de destino.
 * @param path Caminho do arquivo a carregar.
 * @return 1 se o carregamento foi bem-sucedido; 0 se o arquivo não existir ou for inválido.
 */
int samuel_load(SamuelAgent *agent, const char *path);

/**
 * @brief Função adaptadora de tomada de decisão para a interface MoveSelector.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Ponteiro para a instância SamuelAgent.
 * @return Índice da melhor jogada encontrada.
 */
int agent_samuel_move(Board *board, char player, void *context);

#endif
