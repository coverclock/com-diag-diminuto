/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ESCAPE_
#define _H_COM_DIAG_DIMINUTO_ESCAPE_

/**
 * @file
 *
 * Copyright 2013-2015 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Escape feature implements functions to collapse and expand
 * C-style escape sequences. This is especially useful for processing
 * input strings from the terminal or an input file. Portions of this
 * implementation was derived from the Digital Aggregates Grandote
 * C++ library.
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Collapse a source C string containing C-style escape sequences
 * into a destination string containing ASCII characters like CR and LF.
 * The result will be NUL terminated, although if the source string contained
 * one or more '\0' sequences, there may be more than one NUL character in the
 * destination buffer. This function is useful for processing string input
 * from a user terminal or a configuration file.
 * @param to points to the destination buffer.
 * @param from points to the source string.
 * @param tsize is the maximum size of the destination string include the
 * terminating NUL character.
 * @return the length of the destination string including the terminating NUL.
 */
extern size_t diminuto_escape_collapse(char *to, const char *from, size_t tsize);

/**
 * Expand a source string into a destination string containing C-style escape
 * sequences. The result will be NUL terminated. The length
 * of the source string is specified explicitly so that it can contain embedded
 * NUL characters. A set of additional characters to escape can be specified.
 * This is useful for escaping characters like single or double quotes. This
 * function is useful for displaying or storing arbitrary binary data as a
 * string.
 * @param to points to the destination buffer.
 * @param from points to the source string.
 * @param tsize is the maximum size of the destination string include the
 * terminating NUL character.
 * @param fsize is the explicit length of the source string.
 * @param special if non-NULL is a NUL terminated set of additional characters
 * to be escaped.
 * @return the length of the destination string including the terminating NUL.
 */
extern size_t diminuto_escape_expand(char * to, const char * from, size_t tsize, size_t fsize, const char * special);

/**
 * Copy a source C string to a destination string trimming trailing blanks
 * except when the last trailing blank is escaped.
 * @param to points to the destination buffer.
 * @param from points to the source string.
 * @param tsize is the maximum size of the destination string include the
 * terminating NUL character.
 * @param fsize is the explicit length of the source string.
 */
extern size_t diminuto_escape_trim(char * to, const char * from, size_t tsize, size_t fsize);

/**
 * Return true if the source C string contains only printable characters,
 * false otherwise. Printability is defined in an isprint(3) sense.
 * @param from points to the source string.
 * @return true if printable, false otherwise.
 */
extern int diminuto_escape_printable(const char * from);

#endif
