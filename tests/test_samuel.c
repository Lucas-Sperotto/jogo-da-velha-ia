#include "samuel.h"
#include "rng.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_returns_valid_move(void)
{
    Board b;
    SamuelAgent agent;
    board_init(&b);
    samuel_init(&agent);
    int move=agent_samuel_move(&b,PLAYER_O,&agent);
    assert(board_is_valid_move(&b,move));
}

static void test_training_updates_weights(void)
{
    SamuelAgent agent;
    samuel_init(&agent);
    double before=agent.weights.values[1];
    samuel_train(&agent,50);
    assert(agent.weights.values[1] != before);
}

static void test_truncated_load_is_atomic(void)
{
    const char *path="tests/tmp_samuel_bad.dat";
    SamuelAgent agent;
    SamuelAgent before;

    samuel_init(&agent);
    before=agent;

    FILE *file=fopen(path,"w");
    assert(file != NULL);
    fputs("99 88\n",file);
    fclose(file);

    assert(!samuel_load(&agent,path));
    assert(memcmp(&agent,&before,sizeof(agent)) == 0);
    assert(remove(path) == 0);
}

int main(void)
{
    rng_seed(UINT64_C(1001));
    test_returns_valid_move();
    test_training_updates_weights();
    test_truncated_load_is_atomic();
    puts("test_samuel: OK");
    return 0;
}
