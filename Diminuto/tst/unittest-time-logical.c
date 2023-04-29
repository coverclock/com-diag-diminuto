/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a partial unit test of the Time feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a test of the Logical Time feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "../src/diminuto_time.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    SETLOGMASK();

    {
        TEST();

        ASSERT(COM_DIAG_DIMINUTO_TIME_LOGICAL_MAXIMUM == ~(uint64_t)0);
        ASSERT(DIMINUTO_TIME_LOGICAL_MAXIMUM == ~(uint64_t)0);

        STATUS();
    }

    {
        uint64_t logical0;
        uint64_t logical1;
        uint64_t logical2;

        TEST();

        logical0 = diminuto_time_logical();
        ASSERT(logical0 < DIMINUTO_TIME_LOGICAL_MAXIMUM);

        logical1 = diminuto_time_logical();
        ASSERT(logical1 < DIMINUTO_TIME_LOGICAL_MAXIMUM);
        ASSERT(logical1 > logical0);

        logical2 = diminuto_time_logical();
        ASSERT(logical2 < DIMINUTO_TIME_LOGICAL_MAXIMUM);
        ASSERT(logical2 > logical1);

        STATUS();
    }

    {
        uint64_t logical;
        uint64_t logical0;
        int error;

        TEST();

        errno = 0xdeadbeef;
        logical = diminuto_time_logical();
        error = errno;
        ASSERT(logical < DIMINUTO_TIME_LOGICAL_MAXIMUM);
        ASSERT(error == 0);

        errno = 0xc0edbabe;
        logical0 = logical;
        logical = diminuto_time_logical();
        error = errno;
        ASSERT(logical < DIMINUTO_TIME_LOGICAL_MAXIMUM);
        ASSERT(logical > logical0);
        ASSERT(error == 0);

        diminuto_time_logical_set(DIMINUTO_TIME_LOGICAL_MAXIMUM - 1);

        errno = 0xdeadbeef;
        logical0 = logical;
        logical = diminuto_time_logical();
        error = errno;
        ASSERT(logical == (DIMINUTO_TIME_LOGICAL_MAXIMUM - 1));
        ASSERT(logical > logical0);
        ASSERT(error == 0);

        errno = 0xdeadbeef;
        logical0 = logical;
        logical = diminuto_time_logical();
        error = errno;
        ASSERT(logical == DIMINUTO_TIME_LOGICAL_MAXIMUM);
        ASSERT(logical > logical0);
        ASSERT(error == 0);

        errno = 0xdeadbeef;
        logical0 = logical;
        logical = diminuto_time_logical();
        error = errno;
        ASSERT(logical == 0);
        ASSERT(error == EOVERFLOW);

        errno = 0xdeadbeef;
        logical0 = logical;
        logical = diminuto_time_logical();
        error = errno;
        ASSERT(logical == 1);
        ASSERT(logical > logical0);
        ASSERT(error == EOVERFLOW);

        errno = 0xdeadbeef;
        logical0 = logical;
        logical = diminuto_time_logical();
        error = errno;
        ASSERT(logical == 2);
        ASSERT(logical > logical0);
        ASSERT(error == EOVERFLOW);

        diminuto_time_logical_reset();

        errno = 0xdeadbeef;
        logical0 = logical;
        logical = diminuto_time_logical();
        error = errno;
        ASSERT(logical == 3);
        ASSERT(logical > logical0);
        ASSERT(error == 0);

        errno = 0xdeadbeef;
        logical0 = logical;
        logical = diminuto_time_logical();
        error = errno;
        ASSERT(logical == 4);
        ASSERT(logical > logical0);
        ASSERT(error == 0);

        STATUS();
    }

    EXIT();
}
