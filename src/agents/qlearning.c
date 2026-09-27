#include "qlearning.h"

#include <stdio.h>
#include <stdlib.h>

static int encode_state(const Board *board, char player)
{
    int value=0, factor=1;
    char opponent=other_player(player);

    for (int i=0;i<BOARD_SIZE;++i) {
        int digit=0;
        if (board->cells[i] == player) digit=1;
        else if (board->cells[i] == opponent) digit=2;
        value += digit*factor;
        factor *= 3;
    }
    return value;
}

static double *qcell(QLearningAgent *agent, int state, int action)
{
    return &agent->q[state*Q_ACTION_COUNT+action];
}

static double max_q(const QLearningAgent *agent, const Board *board, char player)
{
    int state=encode_state(board,player);
    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    if (count == 0) return 0.0;

    double best=agent->q[state*Q_ACTION_COUNT+moves[0]];
    for (int i=1;i<count;++i) {
        double value=agent->q[state*Q_ACTION_COUNT+moves[i]];
        if (value > best) best=value;
    }
    return best;
}

static int choose_action(QLearningAgent *agent, Board *board, char player, double epsilon)
{
    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    if (count == 0) return -1;

    if (epsilon > 0.0 && ((double)rand()/(double)RAND_MAX) < epsilon)
        return moves[rand()%count];

    int state=encode_state(board,player);
    int best_move=moves[0];
    double best=*qcell(agent,state,best_move);

    for (int i=1;i<count;++i) {
        double value=*qcell(agent,state,moves[i]);
        if (value > best) {
            best=value;
            best_move=moves[i];
        }
    }
    return best_move;
}

static int random_move(Board *board)
{
    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    return count > 0 ? moves[rand()%count] : -1;
}

static void update_q(QLearningAgent *agent, int state, int action,
                     double reward, double next_max)
{
    double *value=qcell(agent,state,action);
    *value += agent->alpha*(reward+agent->gamma*next_max-*value);
}

int qlearning_init(QLearningAgent *agent)
{
    agent->q=calloc((size_t)Q_STATE_COUNT*Q_ACTION_COUNT,sizeof(double));
    if (agent->q == NULL) return 0;
    agent->alpha=0.20;
    agent->gamma=0.95;
    agent->epsilon=0.20;
    agent->episodes=0;
    return 1;
}

void qlearning_free(QLearningAgent *agent)
{
    if (agent == NULL) return;
    free(agent->q);
    agent->q=NULL;
}

void qlearning_train(QLearningAgent *agent, int episodes)
{
    if (agent == NULL || agent->q == NULL || episodes <= 0) return;

    for (int episode=0;episode<episodes;++episode) {
        Board board;
        board_init(&board);
        char learner=(episode%2 == 0) ? PLAYER_X : PLAYER_O;
        char opponent=other_player(learner);

        if (learner == PLAYER_O) {
            int opening=random_move(&board);
            if (opening >= 0) board_make_move(&board,opening,opponent);
        }

        while (!board_is_terminal(&board)) {
            int state=encode_state(&board,learner);
            int action=choose_action(agent,&board,learner,agent->epsilon);
            if (action < 0) break;
            board_make_move(&board,action,learner);

            if (board_winner(&board) == learner) {
                update_q(agent,state,action,1.0,0.0);
                break;
            }
            if (board_is_full(&board)) {
                update_q(agent,state,action,0.0,0.0);
                break;
            }

            int response=random_move(&board);
            if (response >= 0) board_make_move(&board,response,opponent);

            if (board_winner(&board) == opponent) {
                update_q(agent,state,action,-1.0,0.0);
                break;
            }
            if (board_is_full(&board)) {
                update_q(agent,state,action,0.0,0.0);
                break;
            }

            update_q(agent,state,action,0.0,max_q(agent,&board,learner));
        }
        ++agent->episodes;
    }
}

int agent_qlearning_move(Board *board, char player, void *context)
{
    QLearningAgent *agent=context;
    if (agent == NULL || agent->q == NULL) return -1;
    return choose_action(agent,board,player,0.0);
}

int qlearning_save(const QLearningAgent *agent, const char *path)
{
    FILE *file=fopen(path,"wb");
    if (file == NULL) return 0;

    if (fwrite(&agent->episodes,sizeof(agent->episodes),1,file) != 1 ||
        fwrite(agent->q,sizeof(double),(size_t)Q_STATE_COUNT*Q_ACTION_COUNT,file) !=
        (size_t)Q_STATE_COUNT*Q_ACTION_COUNT) {
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

int qlearning_load(QLearningAgent *agent, const char *path)
{
    FILE *file=fopen(path,"rb");
    if (file == NULL) return 0;

    int episodes=0;
    if (fread(&episodes,sizeof(episodes),1,file) != 1 ||
        fread(agent->q,sizeof(double),(size_t)Q_STATE_COUNT*Q_ACTION_COUNT,file) !=
        (size_t)Q_STATE_COUNT*Q_ACTION_COUNT) {
        fclose(file);
        return 0;
    }

    agent->episodes=episodes;
    fclose(file);
    return 1;
}
