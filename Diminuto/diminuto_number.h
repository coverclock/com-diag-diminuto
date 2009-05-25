/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_NUMBER_
#define _H_COM_DIAG_DIMINUTO_NUMBER_

/**
 * @file
 *
 * Copyright 2008-2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdint.h>

/**
 * Parses a string in a strtoul-like manner, returning a pointer to the
 * first non-numeric character. Applications can use this pointer to
 * advance parsing, or to test for the nul end of string character to see
 * if the string was all consumed. If the first non-numeric character is
 * not a nul, errno is set so that the application may choose to print an
 * error message.
 * @param s points to a nul-terminated C-style numeric string in base 8, 10,
 * or 16.
 * @param p points to an unsigned 64-bit integer variable into which the
 * binary result is placed.
 * @return a pointer to the first non-numeric character encountered.
 * @see strtoul(3)
 */
extern const char * diminuto_number(const char * s, uint64_t * p);

#endif
