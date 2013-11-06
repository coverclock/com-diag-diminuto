/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_NUMBER_
#define _H_COM_DIAG_DIMINUTO_NUMBER_

/**
 * @file
 *
 * Copyright 2008-2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Parses a string in a strtoll-like manner, returning a pointer to the
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
 * @see strtoll(3)
 */
extern const char * diminuto_number_unsigned(const char * s, diminuto_unsigned_t * p);

/**
 * Parses a string in a strtoull-like manner, returning a pointer to the
 * first non-numeric character. Applications can use this pointer to
 * advance parsing, or to test for the nul end of string character to see
 * if the string was all consumed. If the first non-numeric character is
 * not a nul, errno is set so that the application may choose to print an
 * error message.
 * @param s points to a nul-terminated C-style numeric string in base 8, 10,
 * or 16.
 * @param p points to an signed 64-bit integer variable into which the
 * binary result is placed.
 * @return a pointer to the first non-numeric character encountered.
 * @see strtoll(3)
 */
extern const char * diminuto_number_signed(const char * s, diminuto_signed_t * p);

#define diminuto_number diminuto_number_unsigned

#endif
