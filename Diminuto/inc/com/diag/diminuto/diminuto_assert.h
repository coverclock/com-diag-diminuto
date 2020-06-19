/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ASSERT_
#define _H_COM_DIAG_DIMINUTO_ASSERT_

/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>
#include <string.h>

/**
 * If the asserted condition is not true, log an error message. If the exit
 * code is non-zero, exit with that code.
 * @param cond is the condition, treated as a boolean.
 * @param string is the stringified condition.
 * @param file is the name of the translation unit.
 * @param line is the line number within the translation unit.
 * @param error is the errno error number.
 * @param code is the exit code.
 */
static inline void diminuto_assert_f(int cond, const char * string, const char * file, int line, int error, int code)
{
    if (!cond) {
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, "%s[%d]: FAIL! (%s) <%d> \"%s\" {%d}\n", file, line, string, error, error ? strerror(error) : "", code);
        if (code) {
            exit(code);
        }
    }
}

/**
 * @def diminuto_assert
 * Calls diminuto_assert_f with @a _COND_ and an exit code.
 */
#define diminuto_assert(_COND_) diminuto_assert_f((_COND_), #_COND_, __FILE__, __LINE__, errno, 1)

/**
 * @def diminuto_assert
 * Calls diminuto_assert_f with @a _COND_ but without an exit code.
 */
#define diminuto_expect(_COND_) diminuto_assert_f((_COND_), #_COND_, __FILE__, __LINE__, errno, 0)

#endif
