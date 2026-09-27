#include "rng.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void test_known_sequence(void)
{
    static const uint32_t expected[]={
        UINT32_C(3184996902),
        UINT32_C(686809907),
        UINT32_C(1196582743),
        UINT32_C(1478287871),
        UINT32_C(163338330)
    };

    rng_seed(UINT64_C(42));
    assert(rng_get_seed() == UINT64_C(42));
    for (size_t i=0;i<sizeof(expected)/sizeof(expected[0]);++i)
        assert(rng_next_u32() == expected[i]);
}

static void test_reseed_reproduces_sequence(void)
{
    uint64_t first[8];

    rng_seed(UINT64_C(123456789));
    for (size_t i=0;i<8;++i) first[i]=rng_next_u64();

    rng_seed(UINT64_C(123456789));
    for (size_t i=0;i<8;++i) assert(rng_next_u64() == first[i]);
}

static void test_ranges(void)
{
    rng_seed(UINT64_C(7));
    for (int i=0;i<1000;++i) {
        assert(rng_index(9) < 9);
        double value=rng_unit();
        assert(value >= 0.0);
        assert(value < 1.0);
    }
    assert(rng_index(0) == 0);
}

int main(void)
{
    test_known_sequence();
    test_reseed_reproduces_sequence();
    test_ranges();
    puts("test_rng: OK");
    return 0;
}
