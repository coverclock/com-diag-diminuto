/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the memory Barrier feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the memory Barrier feature.
 * This is mostly just to insure it compiles.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_barrier.h"

int main(void)
{
    volatile int reg;

    {
        TEST();
#if !defined(COM_DIAG_DIMINUTO_BARRIER)
        FAILURE();
#endif
        STATUS();
    }

    {
        TEST();
#if !defined(COM_DIAG_DIMINUTO_ACQUIRE)
        FAILURE();
#endif
        STATUS();
    }

    {
        TEST();
#if !defined(COM_DIAG_DIMINUTO_RELEASE)
        FAILURE();
#endif
        STATUS();
    }

    {
        TEST();
        diminuto_barrier();
        reg = 0;
        diminuto_barrier();
        reg = 1;
        diminuto_barrier();
        reg = 0;
        STATUS();
    }

    {
        TEST();
        diminuto_acquire();
        reg = 1;
        diminuto_release();
        STATUS();
    }

    {
        TEST();
        diminuto_barrier_f();
        reg = 0;
        diminuto_barrier_f();
        reg = 1;
        diminuto_barrier_f();
        reg = 0;
        STATUS();
    }

    {
        TEST();
        diminuto_acquire_f();
        reg = 1;
        diminuto_release_f();
        STATUS();
    }

    EXIT();
}
