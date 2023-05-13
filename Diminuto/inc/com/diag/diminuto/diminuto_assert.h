/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ASSERT_
#define _H_COM_DIAG_DIMINUTO_ASSERT_

/**
 * @file
 * @copyright Copyright 2020-2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements assert and expect that use the Diminuto log mechanism.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * If COM_DIAG_DIMINUTO_ASSERT_NDEBUG is defined these functions evaluate the
 * condition but do nothing else.
 *
 * If COM_DIAG_DIMINUTO_ASSERT_DEBUG is defined, the asserts are turned into
 * expects (which do everything assert does except the abort).
 *
 * If COM_DIAG_DIMINUTO_ASSERT_FDEBUG is defined, the condition is evaluated
 * and passed to the assert function, which either logs or doesn't, and if
 * it logs, either aborts or doesn't, depending on its arguments.
 *
 * If none are defined, assert and expect work as intended; if the condition
 * is not met, either logs an error, and assert aborts while expect merely
 * merely returns.
 *
 * This file can be included more than once. This is the beginning of the
 * idempotent section of the header.
 */

#include <errno.h>

/**
 * If the asserted condition is not true, log an error message and then if
 * the fatal is true, call abort(). This function is invoked by the macros
 * defined below, but can also be called as a standalone function, passing
 * the condition expression as an argument, which will be evaluated once and
 * only once by the compiler.
 * @param condition is the condition, treated as a boolean.
 * @param string is the stringified condition.
 * @param file is the name of the translation unit.
 * @param line is the line number within the translation unit.
 * @param error is the errno error number.
 * @param fatal if true, causes an abort.
 */
extern void diminuto_assert_f(int condition, const char * string, const char * file, int line, int error, int fatal);

/**
 * @def diminuto_panic
 * Unconditionally cause a fatal error and try to dump core. Note that asserts
 * can be suppressed by a compile-time option, but panics cannot.
 */
#define diminuto_panic() (diminuto_assert_f(0, "PANIC", __FILE__, __LINE__, errno, !0))

/**
 * @def diminuto_contract
 * If @a _CONDITION_ is false then call diminuto_assert_f to abort. Note that
 * asserts can be suppressed by a compile-time option, but contracts cannot.
 */
#define diminuto_contract(_CONDITION_) ((!(_CONDITION_)) ? diminuto_assert_f(0, #_CONDITION_, __FILE__, __LINE__, errno, !0) : ((void)0))

/*
 * End of idempotent section.
 */

#endif

#if defined(diminuto_assert)
#   undef diminuto_assert
#endif

#if defined(diminuto_expect)
#   undef diminuto_expect
#endif

#if defined(NDEBUG)
    /*
     * @def COM_DIAG_DIMINUTO_ASSERT_DEBUG
     * Support for legacy assert(3) behavior by using NDEBUG.
     */
#   define COM_DIAG_DIMINUTO_ASSERT_NDEBUG NDEBUG
#endif

/*
 * N.B. It is important to evaluate the condition once and only once,
 *      since the expression may have side effects. For the same reason,
 *      it is important to evaluate the expression whether the condition
 *      is used or not. The compiler will optimize away code that has no
 *      effect.
 */

#if defined(COM_DIAG_DIMINUTO_ASSERT_NDEBUG)

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

    /**
     * @def diminuto_assert
     * If @a _CONDITION_ is false then call diminuto_assert_f and continue.
     */
#   define diminuto_assert(_CONDITION_) ((!(_CONDITION_)) ? diminuto_assert_f(0, #_CONDITION_, __FILE__, __LINE__, errno, 0) : ((void)0))

    /**
     * @def diminuto_expect
     * If @a _CONDITION_ is false then call diminuto_assert_f and continue.
     */
#   define diminuto_expect(_CONDITION_) ((!(_CONDITION_)) ? diminuto_assert_f(0, #_CONDITION_, __FILE__, __LINE__, errno, 0) : ((void)0))

#elif defined(COM_DIAG_DIMINUTO_ASSERT_FDEBUG)

    /**
     * @def diminuto_assert
     * Call diminuto_assert_f to abort.
     */
#   define diminuto_assert(_CONDITION_) diminuto_assert_f((_CONDITION_), #_CONDITION_, __FILE__, __LINE__, errno, !0)

    /**
     * @def diminuto_expect
     * Call diminuto_assert_f and continue.
     */
#   define diminuto_expect(_CONDITION_) diminuto_assert_f((_CONDITION_), #_CONDITION_, __FILE__, __LINE__, errno, 0)

#else

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

#if !defined(assert)
    /*
     * @def assert
     * Support for legacy assert(3) behavior by defining assert.
     */
#   define assert diminuto_assert
#endif
