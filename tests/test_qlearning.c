#include "qlearning.h"
#include "rng.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void test_truncated_load_is_atomic(void)
{
    const char *path="tests/tmp_qlearning_bad.bin";
    QLearningAgent agent;
    int bad_episodes=999;
    double partial_value=42.0;

    assert(qlearning_init(&agent));
    agent.episodes=17;
    agent.q[0]=1.25;
    agent.q[Q_STATE_COUNT*Q_ACTION_COUNT-1]=-2.5;

    FILE *file=fopen(path,"wb");
    assert(file != NULL);
    assert(fwrite(&bad_episodes,sizeof(bad_episodes),1,file) == 1);
    assert(fwrite(&partial_value,sizeof(partial_value),1,file) == 1);
    fclose(file);

    assert(!qlearning_load(&agent,path));
    assert(agent.episodes == 17);
    assert(agent.q[0] == 1.25);
    assert(agent.q[Q_STATE_COUNT*Q_ACTION_COUNT-1] == -2.5);

    qlearning_free(&agent);
    assert(remove(path) == 0);
}

int main(void)
{
    QLearningAgent agent;
    Board board;

    rng_seed(UINT64_C(3003));
    assert(qlearning_init(&agent));
    qlearning_train(&agent,1000);
    assert(agent.episodes == 1000);

    board_init(&board);
    int move=agent_qlearning_move(&board,PLAYER_X,&agent);
    assert(board_is_valid_move(&board,move));

    qlearning_free(&agent);
    test_truncated_load_is_atomic();
    puts("test_qlearning: OK");
    return 0;
}
