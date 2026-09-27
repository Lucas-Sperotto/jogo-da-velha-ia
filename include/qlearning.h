#ifndef QLEARNING_H
#define QLEARNING_H

#include "game.h"

/**
 * @def Q_STATE_COUNT
 * @brief Número total de estados possíveis na codificação ternária do tabuleiro (3^9 = 19.683).
 *
 * Cada uma das 9 células pode assumir 3 valores sob a perspectiva do jogador:
 *  0 = Vazio, 1 = Peça Própria, 2 = Peça Adversária.
 */
#define Q_STATE_COUNT 19683

/**
 * @def Q_ACTION_COUNT
 * @brief Número total de ações possíveis no tabuleiro (9 posições indexadas de 0 a 8).
 */
#define Q_ACTION_COUNT 9

/**
 * @brief Agente baseado em Aprendizado por Reforço com Tabela Q (Q-Learning Tabular).
 *
 * Aprende a função valor-ação Q(s, a) mapeando cada par (estado, ação) para o retorno
 * esperado cumulativo com desconto temporal.
 */
typedef struct {
    double *q;       /**< Matriz linearizada de tamanho Q_STATE_COUNT * Q_ACTION_COUNT (~141.717 doubles). */
    double alpha;    /**< Taxa de aprendizado (learning rate, ex: 0.20). */
    double gamma;    /**< Fator de desconto para recompensas futuras (discount factor, ex: 0.95). */
    double epsilon;  /**< Taxa de exploração na política epsilon-greedy durante o treino (ex: 0.20). */
    int episodes;    /**< Total de episódios (partidas) treinados. */
} QLearningAgent;

/**
 * @brief Inicializa o agente de Q-Learning, alocando a tabela Q na memória heap e definindo hiperparâmetros.
 *
 * @param agent Ponteiro para a estrutura QLearningAgent.
 * @return 1 se a memória foi alocada com sucesso; 0 em caso de falha de alocação.
 */
int qlearning_init(QLearningAgent *agent);

/**
 * @brief Libera a memória dinâmica alocada para a tabela Q do agente.
 *
 * @param agent Ponteiro para a estrutura QLearningAgent.
 */
void qlearning_free(QLearningAgent *agent);

/**
 * @brief Treina o agente através de episódios de jogo contra um oponente estocástico (aleatório).
 *
 * Alterna entre jogar como primeiro e segundo jogador para aprender políticas completas
 * tanto para X quanto para O usando a equação de Bellman para Q-Learning:
 * Q(s, a) <- Q(s, a) + alpha * [ r + gamma * max_a' Q(s', a') - Q(s, a) ]
 *
 * @param agent Ponteiro para o agente inicializado.
 * @param episodes Quantidade de episódios a serem treinados.
 */
void qlearning_train(QLearningAgent *agent, int episodes);

/**
 * @brief Salva a tabela Q e o número de episódios em formato binário compacto.
 *
 * @param agent Ponteiro para o agente.
 * @param path Caminho do arquivo de destino (ex: "data/qtable.bin").
 * @return 1 se salvo com sucesso; 0 caso ocorra erro de escrita.
 */
int qlearning_save(const QLearningAgent *agent, const char *path);

/**
 * @brief Carrega a tabela Q e o número de episódios a partir de arquivo binário.
 *
 * @param agent Ponteiro para o agente já inicializado.
 * @param path Caminho do arquivo binário a ser lido.
 * @return 1 se carregado com sucesso; 0 se houver erro de leitura ou incompatibilidade.
 */
int qlearning_load(QLearningAgent *agent, const char *path);

/**
 * @brief Função adaptadora de tomada de decisão gananciosa (epsilon=0.0) para MoveSelector.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Ponteiro para QLearningAgent.
 * @return Índice da melhor jogada segundo a tabela Q, ou -1 se não houver jogadas válidas.
 */
int agent_qlearning_move(Board *board, char player, void *context);

#endif
