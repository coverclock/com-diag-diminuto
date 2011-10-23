/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_serial.h"
#include <stdio.h>

int main(void)
{
    int rc;

    rc = diminuto_serial_raw(fileno(stdin));
    ASSERT(rc == 0);

    rc = diminuto_serial_unbuffered(stdout);
    ASSERT(rc == 0);

    EXIT();
}
