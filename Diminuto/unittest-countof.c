/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto/diminuto_unittest.h"
#include "diminuto/diminuto_countof.h"

int main(void)
{
    long array1[16];
    short array2[8];
    char array3[4];
    ASSERT(diminuto_countof(array1) == 16);
    ASSERT(diminuto_countof(array2) == 8);
    ASSERT(diminuto_countof(array3) == 4);
    ASSERT(countof(array1) == 16);
    ASSERT(countof(array2) == 8);
    ASSERT(countof(array3) == 4);
    EXIT();
}
