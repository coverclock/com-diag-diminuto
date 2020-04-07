/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_buffer.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_heap.h"

#define COUNT 1000

static diminuto_ticks_t run(int limit)
{
    diminuto_ticks_t before;
    diminuto_ticks_t after;
    void * pointer[COUNT][10];
    int ii;
    int jj;
    int kk;

    before = diminuto_time_process();

    for (ii = 0; ii < limit; ++ii) {
        for (jj = 0; jj < COUNT; ++jj) {
            for (kk = 0; kk < 10; ++kk) {
                ASSERT((pointer[jj][kk] = diminuto_heap_malloc(1 << (kk + 3))) != (void *)0);
            }
        }
        for (jj = 0; jj < COUNT; ++jj) {
            for (kk = 0; kk < 10; ++kk) {
                diminuto_heap_free(pointer[jj][kk]);
            }
        }
    }

    after = diminuto_time_process();

    return after - before;
}

static void report(diminuto_ticks_t ticks)
{
    int day, hour, minute, second;
    diminuto_ticks_t fraction;

    diminuto_time_duration(ticks, &day, &hour, &minute, &second, &fraction);
    printf("elapsed %lluticks %d/%2.2d:%2.2d:%2.2d.%9.9llu\n", ticks, day, hour, minute, second, (long long unsigned int)fraction);
}

int main(int argc, char * argv[])
{
    int limit = 10000;
    diminuto_ticks_t ticks;

    SETLOGMASK();

    if (argc > 1) {
        limit = strtol(argv[1], (char **)0, 0);
    }

    ASSERT(diminuto_buffer_prealloc(COUNT, 1 << 3) > 0);
    ASSERT(diminuto_buffer_prealloc(COUNT, 1 << 4) > 0);
    ASSERT(diminuto_buffer_prealloc(COUNT, 1 << 5) > 0);
    ASSERT(diminuto_buffer_prealloc(COUNT, 1 << 6) > 0);
    ASSERT(diminuto_buffer_prealloc(COUNT, 1 << 7) > 0);
    ASSERT(diminuto_buffer_prealloc(COUNT, 1 << 8) > 0);
    ASSERT(diminuto_buffer_prealloc(COUNT, 1 << 9) > 0);
    ASSERT(diminuto_buffer_prealloc(COUNT, 1 << 10) > 0);
    ASSERT(diminuto_buffer_prealloc(COUNT, 1 << 11) > 0);
    ASSERT(diminuto_buffer_prealloc(COUNT, 1 << 12) > 0);
    ASSERT(!diminuto_buffer_nomalloc(!0));

    printf("malloc %d %d\n", COUNT, limit);
    ticks = run(limit);
    report(ticks);

    ASSERT(diminuto_heap_malloc_set(diminuto_buffer_malloc) == malloc);
    ASSERT(diminuto_heap_free_set(diminuto_buffer_free) == free);

    printf("diminuto_buffer_malloc %d %d\n", COUNT, limit);
    ticks = run(limit);
    report(ticks);

    diminuto_heap_malloc_set((diminuto_heap_malloc_func_t *)0);
    diminuto_heap_free_set((diminuto_heap_free_func_t *)0);

    diminuto_buffer_fini();
    ASSERT(diminuto_buffer_nomalloc(0));

    EXIT();
}
