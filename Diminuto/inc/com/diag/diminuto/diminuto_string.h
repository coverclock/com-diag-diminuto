/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_STRING_
#define _H_COM_DIAG_DIMINUTO_STRING_

/**
 * @file
 *
 * Copyright 2010-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <string.h>

/**
 * Copy no more than n characters up to and including the
 * terminating NUL character from a source character array
 * to the destination character array. Guarantees that the
 * destination character array is NUL terminated. Does not NUL
 * fill the destination character array in the manner of strncpy().
 * @param dest points to the destination character array.
 * @param src points to the source character array.
 * @param n is the size of the destination character array.
 * @return the pointer to the destination character array.
 */
extern char * diminuto_strscpy(char * dest, const char * src, size_t n);

#endif
