#ifndef REGISTRY_H
#define REGISTRY_H

#include "agents.h"
#include "genetic.h"
#include "qlearning.h"
#include "samuel.h"

typedef enum {
    AGENT_RANDOM = 1,
    AGENT_HEURISTIC = 2,
    AGENT_MINIMAX = 3,
    AGENT_ALPHABETA = 4,
    AGENT_SAMUEL = 5,
    AGENT_GENETIC = 6,
    AGENT_QLEARNING = 7
} AgentKind;

typedef struct {
    AgentKind kind;
    const char *name;
    SearchStats search;
    unsigned long long total_nodes;
    unsigned long long total_prunes;
    SamuelAgent samuel;
    GeneticAgent genetic;
    QLearningAgent qlearning;
    int qlearning_ready;
} RuntimeAgent;

const char *agent_kind_name(AgentKind kind);
int runtime_agent_init(RuntimeAgent *agent, AgentKind kind);
void runtime_agent_destroy(RuntimeAgent *agent);
int runtime_agent_move(Board *board, char player, void *context);

#endif
