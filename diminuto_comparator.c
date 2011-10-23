/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_comparator.h"
#include <string.h>
#include <stdint.h>

int diminuto_compare_strings(const void * a, const void * b)
{
    return strcmp(a, b);
}

int diminuto_compare_pointers(const void * a, const void * b)
{
    return ((intptr_t)a) - ((intptr_t)b);
}
