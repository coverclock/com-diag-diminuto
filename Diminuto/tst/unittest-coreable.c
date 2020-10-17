/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"

int main(int argc, char ** argv)
{
    int *pointer = (int *)0;
    TEST();
    diminuto_core_enable();
    *pointer = 0;
    FAILURE();
    EXIT();
}
