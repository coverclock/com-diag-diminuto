/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
	SETLOGMASK();
	CHECKPOINT("%s", argv[0]);
    EXPECT(!0);
    ASSERT(!0);
    if (fork() == 0) {
        EXPECT(0);
        ASSERT(0);
        exit(10);
    }
    if (fork() == 0) {
        FATAL("FATAL");
        exit(11);
    }
    if (fork() == 0) {
        PANIC("PANIC");
        exit(12);
    }
    diminuto_delay(diminuto_frequency(), 0);
    EXIT();
}

