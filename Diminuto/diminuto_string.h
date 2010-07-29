/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_STRING_
#define _H_COM_DIAG_DIMINUTO_STRING_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_types.h"

/**
 * Copys no more than (n - 1) characters up to and including the
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

/**
 * Identical in semantics to snprintf() but unlike that stdio function,
 * this one guarantees that the resulting character array is NUL terminated.
 * Hence only stores up to (size - 1) characters to leave room for the
 * terminating NUL. Unfortunately does fill the destination character array
 * with NULs if the underlying snprintf() does so.
 * @oaram str points to the destination character array.
 * @param size is the size of the destination character array.
 * @param format points to the format string in the style of printf().
 * @return the number of characters printed not including the trailing NUL,
 * which, as with snprintf(), isn't necessarily size or less.
 */
extern int diminuto_ssprintf(char * str, size_t size, const char * format, ...);

#endif
