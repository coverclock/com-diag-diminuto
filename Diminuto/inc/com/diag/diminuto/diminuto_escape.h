/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ESCAPE_
#define _H_COM_DIAG_DIMINUTO_ESCAPE_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Portions of this implementation was derived from the Digital Aggregates
 * Desperado C++ library.
 * http://www.diag.com/navigation/downloads/Desperado.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Collapse a source C string containing C-style escape sequences like CTRL-r and
 * CTRL-n into a destination string containing ASCII characters like CR and LF.
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
size_t diminuto_escape_collapse(char *to, const char *from, size_t tsize);

/**
 * Expand a source string into a destination string containing C-style escape
 * sequences like CTRL-r and CTRL-n. The result will be NUL terminated. The length
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
size_t diminuto_escape_expand(char * to, const char * from, size_t tsize, size_t fsize, const char * special);

#endif
