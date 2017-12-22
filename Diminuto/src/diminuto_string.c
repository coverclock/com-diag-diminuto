/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_string.h"
#include <stdio.h>
#include <stdarg.h>

char * diminuto_string_copy(char * dest, const char * src, size_t n)
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

DIMINUTO_PROXY_POINTER_C(string, strdup, char *, (const char * s), return, (s))
DIMINUTO_PROXY_POINTER_C(string, strndup, char *, (const char * s, size_t n), return, (s, n))

DIMINUTO_PROXY_SETTOR_C(string, strdup, char *, (const char * s), return, (s))
DIMINUTO_PROXY_SETTOR_C(string, strndup, char *, (const char * s, size_t n), return, (s, n))

DIMINUTO_PROXY_FUNCTION_C(string, strdup, char *, (const char * s), return, (s))
DIMINUTO_PROXY_FUNCTION_C(string, strndup, char *, (const char * s, size_t n), return, (s, n))
