#include "input.h"

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

static void test_int_accepts_valid_values(void)
{
    int value=0;

    assert(parse_int_range("1\n",0,10,&value));
    assert(value == 1);

    assert(parse_int_range("   7  \t\n",0,10,&value));
    assert(value == 7);

    assert(parse_int_range("+10",0,10,&value));
    assert(value == 10);

    assert(parse_int_range("-3",-5,5,&value));
    assert(value == -3);
}

static void test_int_rejects_invalid_suffixes_and_ranges(void)
{
    int value=123;

    assert(!parse_int_range("1abc",0,10,&value));
    assert(!parse_int_range("2.5",0,10,&value));
    assert(!parse_int_range("",0,10,&value));
    assert(!parse_int_range("   ",0,10,&value));
    assert(!parse_int_range("11",0,10,&value));
    assert(!parse_int_range("-1",0,10,&value));
    assert(!parse_int_range("999999999999999999999999999999",INT_MIN,INT_MAX,&value));
    assert(!parse_int_range("1",10,0,&value));
    assert(!parse_int_range("1",0,10,NULL));
}

static void test_u64_accepts_boundaries(void)
{
    uint64_t value=0;

    assert(parse_u64("0",&value));
    assert(value == UINT64_C(0));

    assert(parse_u64("42\n",&value));
    assert(value == UINT64_C(42));

    assert(parse_u64("18446744073709551615",&value));
    assert(value == UINT64_MAX);
}

static void test_u64_rejects_invalid_input(void)
{
    uint64_t value=0;

    assert(!parse_u64("-1",&value));
    assert(!parse_u64("1abc",&value));
    assert(!parse_u64("2.5",&value));
    assert(!parse_u64("",&value));
    assert(!parse_u64("   ",&value));
    assert(!parse_u64("18446744073709551616",&value));
    assert(!parse_u64("1",NULL));
}

int main(void)
{
    test_int_accepts_valid_values();
    test_int_rejects_invalid_suffixes_and_ranges();
    test_u64_accepts_boundaries();
    test_u64_rejects_invalid_input();
    puts("test_input: OK");
    return 0;
}
