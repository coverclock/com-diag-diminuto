/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_core.h"

int main(int argc, char ** argv)
{
    SETLOGMASK();
    TEST();
	ASSERT(diminuto_core_enable() >= 0);
    diminuto_core_fatal();
    FAILURE();
    EXIT();
}
