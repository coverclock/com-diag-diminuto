/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010, 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_comparator.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <string.h>

int diminuto_compare_strings(const void * thisp, const void * thatp)
{
    return strcmp(thisp, thatp);
}

int diminuto_compare_pointers(const void * thisp, const void * thatp)
{
    return ((intptr_t)thisp) - ((intptr_t)thatp);
}
