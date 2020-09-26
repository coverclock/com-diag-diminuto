/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TESTIFY_
#define _H_COM_DIAG_DIMINUTO_TESTIFY_

/**
 * @file
 *
 * Copyright 2013-2017 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Testify feature provides a standard way to interpret strings in
 * configuration files or user input that are intended to be boolean.
 */

/**
 * Interpret a string as indicating true (closed, enabled, high, true, yes,
 * on, 1, 01, 0x1, etc.) or false (open, disabled, low, false, no, off, 0, 0x0).
 * @param testimony points to a C string.
 * @param otherwise is returned verbatim if missing, invalid, ambiguous.
 * @return !0 for true, 0 for false, or otherwise returned verbatim.
 */
extern int diminuto_testify(const char * testimony, int otherwise);

/**
 * @def DIMINUTO_TESTIFY
 * When printing boolean values, I like the convention of printing the value
 * in uppercase ("TRUE" or "FALSE") if what is printed is the default value,
 * and lowercase ("true" or "false") if it isn't. If you want to follow this
 * convention, pass the value returned by telegraph_testify() as @a _VALUE_
 * and the default value as @a _OTHERWISE_ to generate printable strings. The
 * compiler will optimize much of this code away if @a _OTHERWISE_ is a
 * constant. You can also pass the value itself (or its inverse) as @a
 * _OTHERWISE_ to circumvent this convention.
 */
#define DIMINUTO_TESTIFY(_VALUE_, _OTHERWISE_) ((_VALUE_) ? ((_OTHERWISE_) ? "TRUE" : "true") : ((_OTHERWISE_) ? "false" : "FALSE"))

#endif
