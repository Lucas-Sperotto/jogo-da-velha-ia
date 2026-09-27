#include "registry.h"

#include <string.h>

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

void runtime_agent_destroy(RuntimeAgent *agent)
{
    if (agent != NULL && agent->qlearning_ready) {
        qlearning_free(&agent->qlearning);
        agent->qlearning_ready=0;
    }
}

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
