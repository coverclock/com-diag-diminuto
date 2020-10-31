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
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_barrier.h"

/*
 * This is mostly just to insure it compiles.
 */

int main(void)
{
    volatile int reg;
    diminuto_barrier();
    reg = 0;
    diminuto_barrier();
    reg = 1;
    diminuto_barrier();
    diminuto_acquire();
    reg = 0;
    diminuto_release();
    EXIT();
}
