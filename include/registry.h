#ifndef REGISTRY_H
#define REGISTRY_H

#include "agents.h"
#include "genetic.h"
#include "qlearning.h"
#include "samuel.h"

/**
 * @brief Enumeração de todos os tipos de agentes suportados no laboratório.
 */
typedef enum {
    AGENT_RANDOM = 1,    /**< Agente de escolhas puramente aleatórias (baseline). */
    AGENT_HEURISTIC = 2, /**< Agente baseado em regras explícitas. */
    AGENT_MINIMAX = 3,   /**< Agente de busca exaustiva Minimax clássico. */
    AGENT_ALPHABETA = 4, /**< Agente Minimax com otimização de Poda Alpha-Beta. */
    AGENT_SAMUEL = 5,    /**< Agente de auto-jogo estilo Arthur Samuel. */
    AGENT_GENETIC = 6,   /**< Agente treinado via Algoritmo Genético. */
    AGENT_QLEARNING = 7  /**< Agente por Reforço com Tabela Q. */
} AgentKind;

/**
 * @brief Estrutura unificada de execução de agentes em tempo de execução.
 *
 * Encapsula o estado interno de qualquer tipo de agente, gerencia a memória necessária
 * (como a tabela Q) e acumula estatísticas de desempenho (nós de busca, podas).
 */
typedef struct {
    AgentKind kind;                   /**< Identificador do tipo do agente. */
    const char *name;                 /**< Nome textual amigável do agente. */
    SearchStats search;               /**< Estatísticas da última jogada (Minimax/Alpha-Beta). */
    unsigned long long total_nodes;   /**< Total acumulado de nós visitados ao longo de partidas. */
    unsigned long long total_prunes;  /**< Total acumulado de podas ao longo de partidas. */
    SamuelAgent samuel;               /**< Estrutura interna caso o agente seja AGENT_SAMUEL. */
    GeneticAgent genetic;             /**< Estrutura interna caso o agente seja AGENT_GENETIC. */
    QLearningAgent qlearning;         /**< Estrutura interna caso o agente seja AGENT_QLEARNING. */
    int qlearning_ready;              /**< Flag booleana indicando se qlearning foi alocado. */
} RuntimeAgent;

/**
 * @brief Retorna o nome amigável em formato de texto para um determinado tipo de agente.
 *
 * @param kind Tipo do agente (AgentKind).
 * @return String constante com o nome amigável (ex: "Alpha-Beta").
 */
const char *agent_kind_name(AgentKind kind);

/**
 * @brief Inicializa uma instância de RuntimeAgent para o tipo informado.
 *
 * Caso o agente requeira pesos ou tabelas pré-treinadas (Samuel, Genético, Q-Learning),
 * tenta carregá-los do disco (`data/`). Se não existirem, executa o treinamento automático
 * e salva os artefatos no disco para execuções futuras.
 *
 * @param agent Ponteiro para a instância RuntimeAgent.
 * @param kind Tipo do agente a inicializar.
 * @return 1 se inicializado com sucesso; 0 em caso de erro (ex: falha de alocação de memória).
 */
int runtime_agent_init(RuntimeAgent *agent, AgentKind kind);

/**
 * @brief Libera recursos dinâmicos alocados pelo agente em runtime_agent_init.
 *
 * @param agent Ponteiro para a instância a ser finalizada.
 */
void runtime_agent_destroy(RuntimeAgent *agent);

/**
 * @brief Função polimórfica de despacho de jogada para o agente em tempo de execução.
 *
 * Compatível com a assinatura `MoveSelector`, despacha para o algoritmo correto com base
 * no campo `kind` e atualiza contadores de métricas de busca.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Ponteiro para a estrutura RuntimeAgent.
 * @return Índice da jogada (0 a 8) ou -1 se inválido.
 */
int runtime_agent_move(Board *board, char player, void *context);

#endif
