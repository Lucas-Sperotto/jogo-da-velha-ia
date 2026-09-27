#include "registry.h"

#include <string.h>

/**
 * @brief Converte o enum AgentKind na sua representação em texto amigável.
 *
 * @param kind Tipo do agente (AgentKind).
 * @return String constante estática com o nome em português.
 */
const char *agent_kind_name(AgentKind kind)
{
    switch (kind) {
        case AGENT_RANDOM: return "Aleatório";
        case AGENT_HEURISTIC: return "Heurístico";
        case AGENT_MINIMAX: return "Minimax";
        case AGENT_ALPHABETA: return "Alpha-Beta";
        case AGENT_SAMUEL: return "Samuel-style";
        case AGENT_GENETIC: return "Genético";
        case AGENT_QLEARNING: return "Q-Learning";
        default: return "Desconhecido";
    }
}

/**
 * @brief Inicializa e prepara um agente para execução em tempo real.
 *
 * Configura os campos base, zera estatísticas de busca e, para agentes com aprendizado:
 *  - Samuel: carrega de "data/samuel_weights.dat" ou treina 5000 jogos e salva;
 *  - Genético: carrega de "data/genetic_weights.dat" ou evolui 60 gerações e salva;
 *  - Q-Learning: aloca tabela Q, carrega de "data/qtable.bin" ou treina 50000 episódios e salva.
 *
 * @param agent Ponteiro para a estrutura RuntimeAgent.
 * @param kind Tipo de agente a instanciar.
 * @return 1 se inicializado com sucesso; 0 em caso de erro de argumentos ou alocação.
 */
int runtime_agent_init(RuntimeAgent *agent, AgentKind kind)
{
    if (agent == NULL || kind < AGENT_RANDOM || kind > AGENT_QLEARNING) return 0;
    memset(agent,0,sizeof(*agent));
    agent->kind=kind;
    agent->name=agent_kind_name(kind);

    switch (kind) {
        case AGENT_SAMUEL:
            samuel_init(&agent->samuel);
            if (!samuel_load(&agent->samuel,"data/samuel_weights.dat")) {
                samuel_train(&agent->samuel,5000);
                (void)samuel_save(&agent->samuel,"data/samuel_weights.dat");
            }
            break;
        case AGENT_GENETIC:
            genetic_init(&agent->genetic);
            if (!genetic_load(&agent->genetic,"data/genetic_weights.dat")) {
                genetic_train(&agent->genetic,60);
                (void)genetic_save(&agent->genetic,"data/genetic_weights.dat");
            }
            break;
        case AGENT_QLEARNING:
            if (!qlearning_init(&agent->qlearning)) return 0;
            agent->qlearning_ready=1;
            if (!qlearning_load(&agent->qlearning,"data/qtable.bin")) {
                qlearning_train(&agent->qlearning,50000);
                (void)qlearning_save(&agent->qlearning,"data/qtable.bin");
            }
            break;
        default:
            break;
    }
    return 1;
}

/**
 * @brief Libera recursos dinâmicos alocados pelo agente durante a execução.
 *
 * Especificamente, desaloca a tabela Q do QLearningAgent se ela estiver ativa.
 *
 * @param agent Ponteiro para o agente a ser destruído.
 */
void runtime_agent_destroy(RuntimeAgent *agent)
{
    if (agent != NULL && agent->qlearning_ready) {
        qlearning_free(&agent->qlearning);
        agent->qlearning_ready=0;
    }
}

/**
 * @brief Despachador polimórfico de jogada para RuntimeAgent.
 *
 * Identifica o tipo do agente via campo 'kind', invoca o algoritmo correspondente,
 * e acumula métricas de nós visitados e podas realizadas para Minimax/Alpha-Beta.
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Ponteiro para o RuntimeAgent.
 * @return Índice da jogada (0 a 8) ou -1 se inválido.
 */
int runtime_agent_move(Board *board, char player, void *context)
{
    RuntimeAgent *agent=context;
    if (agent == NULL) return -1;

    int move=-1;
    switch (agent->kind) {
        case AGENT_RANDOM:
            move=agent_random_move(board,player,NULL);
            break;
        case AGENT_HEURISTIC:
            move=agent_heuristic_move(board,player,NULL);
            break;
        case AGENT_MINIMAX:
            move=agent_minimax_move(board,player,&agent->search);
            agent->total_nodes += agent->search.nodes;
            break;
        case AGENT_ALPHABETA:
            move=agent_alphabeta_move(board,player,&agent->search);
            agent->total_nodes += agent->search.nodes;
            agent->total_prunes += agent->search.prunes;
            break;
        case AGENT_SAMUEL:
            move=agent_samuel_move(board,player,&agent->samuel);
            break;
        case AGENT_GENETIC:
            move=agent_genetic_move(board,player,&agent->genetic);
            break;
        case AGENT_QLEARNING:
            move=agent_qlearning_move(board,player,&agent->qlearning);
            break;
        default:
            break;
    }
    return move;
}
