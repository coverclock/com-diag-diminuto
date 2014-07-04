/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

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
    return 0;
}
