/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2013-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Memory feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Memory feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_memory.h"
#include <stdio.h>

int main(void)
{
    size_t pagesize;
    size_t linesize;
    size_t pagesize2;
    size_t linesize2;
    size_t candidate;
    size_t size;
    size_t alignment;
    size_t power;
    size_t allocated;
    int method;
    void * pointer;

    SETLOGMASK();

    {
        TEST();

        method = -1;
        pagesize = diminuto_memory_pagesize(&method);
        pagesize2 = diminuto_memory_pagesize((int *)0);
        CHECKPOINT("pagesize=0x%x=%d method=%d\n", pagesize, pagesize, method);
        ASSERT(method >= 0);
        ASSERT(pagesize > 0);
        ASSERT(pagesize == pagesize2);
        ASSERT(diminuto_memory_is_power(pagesize));

        STATUS();
    }

    {
        TEST();

        method = -1;
        linesize = diminuto_memory_linesize(&method);
        linesize2 = diminuto_memory_linesize((int *)0);
        CHECKPOINT("linesize=0x%x=%d method=%d\n", linesize, linesize, method);
        ASSERT(method >= 0);
        ASSERT(linesize > 0);
        ASSERT(linesize == linesize2);
        ASSERT(diminuto_memory_is_power(linesize));

        STATUS();
    }

    {
        TEST();

        ASSERT(!diminuto_memory_is_power(0));
        power = diminuto_memory_power(0);
        ASSERT(power == 1);
        ASSERT(diminuto_memory_is_power(power));

        for (candidate = 1; candidate > 0; candidate <<= 1) {
            ASSERT(diminuto_memory_is_power(candidate));
            power = diminuto_memory_power(candidate);
            ASSERT(power == candidate);
            ASSERT(diminuto_memory_is_power(power));
        }

        for (candidate = 0x3; candidate < 0x80000000; candidate = (candidate << 1) | 1) {
            ASSERT(!diminuto_memory_is_power(candidate));
            power = diminuto_memory_power(candidate);
            ASSERT(power > candidate);
            ASSERT(diminuto_memory_is_power(power));
        }

        for (candidate = 0; candidate <= pagesize; ++candidate) {
            power = diminuto_memory_power(candidate);
            ASSERT(power >= candidate);
            ASSERT(diminuto_memory_is_power(power));
        }

        pagesize2 = pagesize * 2;
        linesize2 = linesize * 2;
        for (size = 1; size <= pagesize2; ++size) {
            for (alignment = 1; alignment <= linesize2; ++alignment) {
                power = diminuto_memory_power(alignment);
                allocated = diminuto_memory_alignment(size, power);
                ASSERT(allocated >= size);
                ASSERT(allocated <= pagesize2);
                ASSERT((allocated % power) == 0);
            }
        }

        pointer = diminuto_memory_aligned(2, 3);
        ASSERT(pointer != (void *)0);
        free(pointer);

        STATUS();
    }

    EXIT();
}
