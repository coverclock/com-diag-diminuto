/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * EXPERIMENTAL * UNTESTED
 */

#include "com/diag/diminuto/diminuto_reaper.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int xc = 2;

    do {
        if (diminuto_reaper_install(!0) < 0) { break; }
        if (diminuto_terminator_install(0) < 0) { break; }
        while (!diminuto_terminator_check()) {
            (void)pause();
        }
        xc = 0;
    } while (0);

    exit(xc);
}
