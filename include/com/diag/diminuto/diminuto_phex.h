/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PHEX_
#define _H_COM_DIAG_DIMINUTO_PHEX_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Arvada CO USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Emits characters to a FILE, encoding non-printable characters as if they
 * were written for a C program. Automatically inserts newlines. When printing
 * in hexadecimal, two hexadecimal digits are always printed, eliminating any
 * ambiguity. "phex" is pronounced "fex" but intended to mean "print hex".
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>

/**
 * Handle the logic of knowing when to end the current output line. This is
 * exposed so that it can be used for other purposes, but is used internally
 * by the emit function. It can be used by the application to force a
 * termination of an output line early by calling it with an increment that is
 * a negative number. Calling it with an increment that is zero terminates the
 * output line if and only if at least one character has been emitted on it.
 * Otherwise, the output line is terminated if this increment would cause the
 * number of emitted characters to exceed the specified length.
 * @param fp points to a FILE to which the output is emitted.
 * @param length is the maximum length of an output line.
 * @param increment is how many characters are going to be emitted.
 * @param currentp points to a state variable initialized to zero.
 * @param endp points to a state variable initialized to zero.
 */
extern void diminuto_phex_limit(FILE * fp, const size_t length, ssize_t increment, size_t * currentp, int * endp);

/**
 * Emit a character to an output FILE in a printable form that is more or less
 * how C represents the character in a translation unit. Handle inserting
 * newlines where necessary.
 * @param fp points to a FILE to which output is emitted.
 * @param ch is the character to be emitted.
 * @param length is the maximum length of an output line.
 * @param nl if true causes newline character not to be escaped (try false).
 * @param esc if true causes ", ', and ? to be escaped (try false).
 * @param hex if true causes normally escaped characters to be printed in hex (try false).
 * @param currentp points to a state variable initialized to zero.
 * @param endp points to a state variable initialized to zero.
 * @param flush if true causes the FILE to be flushed at the end of each emit (try true).
 */
extern void diminuto_phex_emit(FILE * fp, unsigned char ch, size_t length, int nl, int esc, int hex, size_t * currentp, int * endp, int flush);

#endif
