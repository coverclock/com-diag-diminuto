/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_buffer.h"

/*
 * [0]: 2^3  =    8 + 8 =   16
 * [1]: 2^4  =   16 + 8 =   24
 * [2]: 2^5  =   32 + 8 =   40
 * [3]: 2^6  =   64 + 8 =   72
 * [4]: 2^7  =  128 + 8 =  136
 * [5]: 2^8  =  256 + 8 =  264
 * [6]: 2^9  =  512 + 8 =  520
 * [7]: 2^10 = 1024 + 8 = 1032
 * [8]: 2^11 = 2048 + 8 = 2056
 * [9]: 2^12 = 4096 + 8 = 4104
 */

int main(void)
{
    SETLOGMASK();

    {
        size_t requested;
        size_t actual;
        size_t expected;
        size_t effective;
        unsigned int index;
        unsigned int hash;
        for (requested = 0; requested <= 8192; ++requested) {
            if (requested <= (expected = 8)) {
                index = 0;
            } else if (requested <= (expected = 16)) {
                index = 1;
            } else if (requested <= (expected = 32)) {
                index = 2;
            } else if (requested <= (expected = 64)) {
                index = 3;
            } else if (requested <= (expected = 128)) {
                index = 4;
            } else if (requested <= (expected = 256)) {
                index = 5;
            } else if (requested <= (expected = 512)) {
                index = 6;
            } else if (requested <= (expected = 1024)) {
                index = 7;
            } else if (requested <= (expected = 2048)) {
                index = 8;
            } else if (requested <= (expected = 4096)) {
                index = 9;
            } else {
                expected = requested;
                index = 10;
            }
            expected += 8;
            effective = diminuto_buffer_effective(index);
            hash = diminuto_buffer_hash(requested, &actual);
#if 0
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "%zu %zu %zu %d %d\n", requested, actual, expected, index, hash);
#endif
            EXPECT((index > 9) || (hash == index));
            EXPECT(actual == expected);
            EXPECT((index > 9) || (effective == expected));
        }
        STATUS();
    }

    {
        diminuto_buffer_fini();
        diminuto_buffer_log();
        diminuto_buffer_fini();
        STATUS();
    }

    {
        int index;
        size_t requested;
        void * buffer[15][2] = { (void *)0 };
        diminuto_buffer_log();
        for (index = 0, requested = 0; requested <= (1 << 13); ++index, requested = (requested == 0) ? 1 : (requested << 1)) {
            buffer[index][0] = diminuto_buffer_malloc(requested);
            EXPECT(buffer[index] != (void *)0);
            buffer[index][1] = diminuto_buffer_malloc(requested);
            EXPECT(buffer[index] != (void *)0);
#if 0
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "%d %zu %p %p\n", index, requested, buffer[index][0], buffer[index][1]);
#endif
        }
        diminuto_buffer_log();
        for (--index; index >= 0; --index) {
#if 0
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "%d %p %p\n", index, buffer[index][0], buffer[index][1]);
#endif
            diminuto_buffer_free(buffer[index][0]);
            diminuto_buffer_free(buffer[index][1]);
        }
        diminuto_buffer_log();
        diminuto_buffer_fini();
        diminuto_buffer_log();
        STATUS();
    }

    EXIT();
}
