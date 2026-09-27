#include "input.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

static int only_trailing_space(const char *text)
{
    while (*text != '\0') {
        if (!isspace((unsigned char)*text)) return 0;
        ++text;
    }
    return 1;
}

int parse_int_range(const char *text, int min, int max, int *out)
{
    char *end=NULL;
    long value;

    if (text == NULL || out == NULL || min > max) return 0;

    errno=0;
    value=strtol(text,&end,10);

    if (end == text || errno == ERANGE) return 0;
    if (!only_trailing_space(end)) return 0;
    if (value < (long)min || value > (long)max) return 0;

    *out=(int)value;
    return 1;
}

int parse_u64(const char *text, uint64_t *out)
{
    const char *start=text;
    char *end=NULL;
    uintmax_t value;

    if (text == NULL || out == NULL) return 0;

    while (*start != '\0' && isspace((unsigned char)*start)) ++start;
    if (*start == '-' || *start == '\0') return 0;

    errno=0;
    value=strtoumax(start,&end,10);

    if (end == start || errno == ERANGE) return 0;
    if (!only_trailing_space(end)) return 0;
    if (value > UINT64_MAX) return 0;

    *out=(uint64_t)value;
    return 1;
}
