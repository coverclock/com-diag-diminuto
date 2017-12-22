/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_STRING_
#define _H_COM_DIAG_DIMINUTO_STRING_

/**
 * @file
 *
 * Copyright 2010-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * IMPORTANT SAFETY TIP: depending on the implementation, any alternative
 * implementations of malloc(3) et al. used by this strdup may be incompatible.
 * So if, for example, you install allocate some memory, then install an
 * alternative free(3), then free that memory (or vice versa), wackiness will
 * likely ensue.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_proxy.h"
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
extern char * diminuto_string_copy(char * dest, const char * src, size_t n);

static inline char * diminuto_strscpy(char * dest, const char * src, size_t n) {
	return diminuto_string_copy(dest, src, n);
}

/**
 * Return a pointer to the basename of the specified pathname. When the
 * argv[0] of a main program is passed as the parameter, this returns a
 * pointer to the program name without the leading path.
 * @param argvzero is the pathname.
 * @return a pointer to the basename in the specified pathname.
 */
static inline const char * diminuto_string_basename(const char * argvzero) {
	const char * basename;
	return ((basename = strrchr(argvzero, '/')) == (char *)0) ? argvzero : basename + 1;
}

DIMINUTO_PROXY_POINTER_H(string, strdup, char *, (const char * s), return, (s))
DIMINUTO_PROXY_POINTER_H(string, strndup, char *, (const char * s, size_t n), return, (s, n))

DIMINUTO_PROXY_SETTOR_H(string, strdup, char *, (const char * s), return, (s))
DIMINUTO_PROXY_SETTOR_H(string, strndup, char *, (const char * s, size_t n), return, (s, n))

DIMINUTO_PROXY_FUNCTION_H(string, strdup, char *, (const char * s), return, (s))
DIMINUTO_PROXY_FUNCTION_H(string, strndup, char *, (const char * s, size_t n), return, (s, n))

#endif
