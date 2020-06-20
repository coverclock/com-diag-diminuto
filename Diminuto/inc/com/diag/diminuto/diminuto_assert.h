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

#include <errno.h>

/**
 * If the asserted condition is not true, log an error message and then if
 * the flag is true, call abort();
 * @param condition is the condition, treated as a boolean.
 * @param string is the stringified condition.
 * @param file is the name of the translation unit.
 * @param line is the line number within the translation unit.
 * @param error is the errno error number.
 * @param flag if true, causes an abort.
 */
extern void diminuto_assert_f(int condition, const char * string, const char * file, int line, int error, int flag);

/*
 * N.B. It is important to evaluate the condition once and only once,
 *      since the expression may have side effects.
 */

/**
 * @def diminuto_assert
 * If @a _CONDITION_ is false then call diminuto_assert_f with an exit code.
 */
#define diminuto_assert(_CONDITION_) ((!(_CONDITION_)) ? diminuto_assert_f(0, #_CONDITION_, __FILE__, __LINE__, errno, !0) : ((void)0))

/**
 * @def diminuto_expect
 * If @a _CONDITION_ is false then call diminuto_assert_f with no exit code.
 */
#define diminuto_expect(_CONDITION_) ((!(_CONDITION_)) ? diminuto_assert_f(0, #_CONDITION_, __FILE__, __LINE__, errno, 0) : ((void)0))

#endif
