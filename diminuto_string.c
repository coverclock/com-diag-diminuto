/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_string.h"

char * diminuto_strscpy(char * dest, const char * src, size_t n)
{
    char * result;

    for (result = dest; (n > 1) && (*src != '\0'); --n) {
        *(dest++) = *(src++);
    }

    if (n > 0) {
        *dest = '\0';
    }

    return result;
}
