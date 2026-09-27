#include "rng.h"

#include <time.h>

static uint64_t rng_state=0;
static uint64_t rng_seed_value=0;
static int rng_initialized=0;

void rng_seed(uint64_t seed)
{
    rng_seed_value=seed;
    rng_state=seed;
    rng_initialized=1;
}

uint64_t rng_seed_auto(void)
{
    struct timespec now={0};
    uint64_t seed;

    if (timespec_get(&now,TIME_UTC) == TIME_UTC) {
        seed=(uint64_t)now.tv_sec;
        seed ^= (uint64_t)now.tv_nsec << 32;
    } else {
        seed=(uint64_t)time(NULL);
    }
    seed ^= (uint64_t)(unsigned long)clock();
    rng_seed(seed);
    return seed;
}

uint64_t rng_get_seed(void)
{
    if (!rng_initialized) (void)rng_seed_auto();
    return rng_seed_value;
}

uint64_t rng_next_u64(void)
{
    uint64_t z;
    if (!rng_initialized) (void)rng_seed_auto();

    rng_state += UINT64_C(0x9E3779B97F4A7C15);
    z=rng_state;
    z=(z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z=(z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}

uint32_t rng_next_u32(void)
{
    return (uint32_t)(rng_next_u64() >> 32);
}

size_t rng_index(size_t bound)
{
    if (bound == 0) return 0;

    uint64_t b=(uint64_t)bound;
    uint64_t threshold=(uint64_t)(-b) % b;
    uint64_t value;

    do {
        value=rng_next_u64();
    } while (value < threshold);

    return (size_t)(value % b);
}

double rng_unit(void)
{
    return (double)(rng_next_u64() >> 11) * (1.0 / 9007199254740992.0);
}
