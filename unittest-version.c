/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto/diminuto_version.h"
#include "diminuto/diminuto_unittest.h"
#include <stdio.h>

int main(void)
{
    fputs(DIMINUTO_VERSION, stdout);
    fputc('\n', stdout);
}
