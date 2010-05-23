/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_endianess.h"
#include <stdio.h>

int main(void)
{
    int rc;
	printf("%s-endian\n", (rc = diminuto_littleendian()) ? "little" : "big");
    return rc;
}
