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
 *
 * Implements assert and expect functions that under the hood use the
 * Diminuto log mechanism.
 *
 * If COM_DIAG_DIMINUTO_ASSERT_NDEBUG is defined (which should happen only
 * under the most exceptional of circumstances and perhaps only for debugging),
 *  these functions evaluate the condition but do nothing else.
 *
 * If COM_DIAG_DIMINUTO_ASSERT_DEBUG is defined, the asserts are turned into
 * expects (which do everything assert does except the abort).
 *
 * If neither is defined, assert and expect work as intended; if the
 * condition is not met, either logs an error, and assert aborts while
 * expect merely returns.
 *
 * This file can be included more than once.
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

#endif

#if defined(diminuto_assert)
#   undef diminuto_assert
#endif

#if defined(diminuto_expect)
#   undef diminuto_expect
#endif

#if defined(COM_DIAG_DIMINUTO_ASSERT_NDEBUG)

/*
 * As bizzare as it sounds, we still evaluate the condition in order
 * to produce any side effects, then throw the result away. We let the
 * compiler optimize away the code if there is nothing to be achieved.
 */

/**
 * @def diminuto_assert
 * Evaluate @a _CONDITION_ and throw the result away.
 */
#   define diminuto_assert(_CONDITION_) ((void)(_CONDITION_))

/**
 * @def diminuto_expect
 * Evaluate @a _CONDITION_ and throw the result away.
 */
#   define diminuto_expect(_CONDITION_) ((void)(_CONDITION_))

#elif defined(COM_DIAG_DIMINUTO_ASSERT_DEBUG)

/*
 * N.B. It is important to evaluate the condition once and only once,
 *      since the expression may have side effects.
 */

/**
 * @def diminuto_assert
 * If @a _CONDITION_ is false then call diminuto_assert_f to abort.
 */
#   define diminuto_assert(_CONDITION_) ((!(_CONDITION_)) ? diminuto_assert_f(0, #_CONDITION_, __FILE__, __LINE__, errno, 0) : ((void)0))

/**
 * @def diminuto_expect
 * If @a _CONDITION_ is false then call diminuto_assert_f and continue.
 */
#   define diminuto_expect(_CONDITION_) ((!(_CONDITION_)) ? diminuto_assert_f(0, #_CONDITION_, __FILE__, __LINE__, errno, 0) : ((void)0))

#else

/*
 * N.B. It is important to evaluate the condition once and only once,
 *      since the expression may have side effects.
 */

/**
 * @def diminuto_assert
 * If @a _CONDITION_ is false then call diminuto_assert_f to abort.
 */
#   define diminuto_assert(_CONDITION_) ((!(_CONDITION_)) ? diminuto_assert_f(0, #_CONDITION_, __FILE__, __LINE__, errno, !0) : ((void)0))

/**
 * @def diminuto_expect
 * If @a _CONDITION_ is false then call diminuto_assert_f and continue.
 */
#   define diminuto_expect(_CONDITION_) ((!(_CONDITION_)) ? diminuto_assert_f(0, #_CONDITION_, __FILE__, __LINE__, errno, 0) : ((void)0))

#endif
