/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_UNITTEST_
#define _H_COM_DIAG_DIMINUTO_UNITTEST_

/**
 * @file
 *
 * Copyright 2009-2017 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Unit Test feature is a dirt simple unit testing framework for C
 * programs. If you are using C++ (or even if you are using C but are
 * comfortable with C++) you would be better off using one of the
 * established C++ unit test frameworks. I particularly like Google
 * Test (a.k.a. gtest). Also, developers I know and trust have made good
 * use of CxxUnit, CppUnit, and CxxUnitLite. If you are using Java,
 * go directly to JUnit.
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

extern int diminuto_unittest_test;
extern int diminuto_unittest_tests;
extern int diminuto_unittest_errors;
extern int diminuto_unittest_total;
extern int diminuto_unittest_errno;

/**
 * @def SETLOGMASK()
 * Set the log mask from the string value of the environmental variable
 * DIMINUTO_LOG_MASK. The value of the environmental variable can be in decimal,
 * hexadecimal, or octal in strtoul() format.
 */
#define SETLOGMASK() \
    diminuto_log_setmask()

/**
 * @def TEST(...)
 * Emit a notice message identifying the the start of a new unit test
 * and an optional manifest string argument.
 */
#define TEST(...) \
    do { \
        diminuto_unittest_test += 1; \
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE "TEST: test=%d " __VA_ARGS__ "\n", diminuto_unittest_test); \
        fflush(stdout); \
        fflush(stderr); \
        diminuto_unittest_errors = 0; \
        diminuto_unittest_tests += 1; \
    } while (0)

/**
 * @def CHECKPOINT(...)
 * Emit a notice message with the current translation unit file and line nunber.
 */
#define CHECKPOINT(...) \
    diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE "CHECKPOINT: " __VA_ARGS__)

/**
 * @def COMMENT(...)
 * Emit a debug message with the current translation unit file and line number.
 */
#define COMMENT(...) \
    diminuto_log_log(DIMINUTO_LOG_PRIORITY_DEBUG, DIMINUTO_LOG_HERE "COMMENT: " __VA_ARGS__)

/**
 * @def STATUS()
 * Emit a count of the  errors so far and an optional manifest string argument.
 */
#define STATUS(...) \
    do { \
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE "STATUS: test=%d errors=%d total=%d %s " __VA_ARGS__ "\n", diminuto_unittest_test, diminuto_unittest_errors, diminuto_unittest_total, (diminuto_unittest_errors == 0) ? "SUCCESS." : "FAILURE!"); \
        fflush(stdout); \
        fflush(stderr); \
    } while (0)

/**
 * @def FAILURE()
 * Increment the error counter and emit a warning message and an optional
 * manifest string argument.
 */
#define FAILURE(...) \
    do { \
        ++diminuto_unittest_errors; \
        ++diminuto_unittest_total; \
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_WARNING, DIMINUTO_LOG_HERE "FAILURE: test=%d errors=%d total=%d %s " __VA_ARGS__ "\n", diminuto_unittest_test, diminuto_unittest_errors, diminuto_unittest_total, "FAILURE!"); \
    } while (0)

/**
 * @def EXIT(...)
 * Emit  a notice message with an optional manifest string argument.
 * Exit the calling process with a zero exit code if there are no errors,
 * or a non-zero exit code if there are.
 */
#define EXIT(...) \
    do { \
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE "EXIT: tests=%d errors=%d %s " __VA_ARGS__ "\n", diminuto_unittest_tests, diminuto_unittest_total, (diminuto_unittest_total == 0) ? "SUCCESS." : "FAILURE!"); \
        fflush(stdout); \
        fflush(stderr); \
        exit(diminuto_unittest_total > 255 ? 255 : diminuto_unittest_total); \
    } while (0)

/**
 * @def FATAL(...)
 * Emit a error message and an optional manifest string argument and exit the
 * calling process with a non-zero exit code.
 */
#define FATAL(...) \
    do { \
        ++diminuto_unittest_errors; \
        ++diminuto_unittest_total; \
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_WARNING, DIMINUTO_LOG_HERE "FATAL: test=%d errors=%d total=%d %s " __VA_ARGS__ "\n", diminuto_unittest_test, diminuto_unittest_errors, diminuto_unittest_total, "FAILURE!"); \
        fflush(stdout); \
        fflush(stderr); \
        exit(diminuto_unittest_total > 255 ? 255 : diminuto_unittest_total); \
    } while (0)

/**
 * @def PANIC(...)
 * Emit a error message and an optional manifest string argument. Try to dump
 * core.
 */
#define PANIC(...) \
    do { \
        ++diminuto_unittest_errors; \
        ++diminuto_unittest_total; \
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, DIMINUTO_LOG_HERE "PANIC: test=%d errors=%d total=%d %s " __VA_ARGS__ "\n", diminuto_unittest_test, diminuto_unittest_errors, diminuto_unittest_total, "FAILURE!"); \
        fflush(stdout); \
        fflush(stderr); \
        diminuto_core_enable(); \
        diminuto_core_fatal(); \
        *((volatile char *)0); \
        exit(diminuto_unittest_total > 255 ? 255 : diminuto_unittest_total); \
    } while (0)

/**
 * @def ADVISE(_COND_)
 * Emit a notice message if the specified condition @a _COND_ is not true.
 */
#define ADVISE(_COND_) \
    do { \
        diminuto_unittest_errno = errno; \
        if (!(_COND_)) { \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE "ADVISE: test=%d errors=%d total=%d !(%s).\n", diminuto_unittest_test, diminuto_unittest_errors, diminuto_unittest_total, #_COND_); \
            if (diminuto_unittest_errno) { \
                diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE "ADVISE: errno=%d=\"%s\".\n", diminuto_unittest_errno, strerror(diminuto_unittest_errno)); \
            } \
        } \
    } while (0)

/**
 * @def EXPECT(_COND_)
 * Emit a warning message if the specified condition @a _COND_ is not true
 * and increment the error counter.
 */
#define EXPECT(_COND_) \
    do { \
        diminuto_unittest_errno = errno; \
        if (!(_COND_)) { \
            ++diminuto_unittest_errors; \
            ++diminuto_unittest_total; \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_WARNING, DIMINUTO_LOG_HERE "EXPECT: test=%d errors=%d total=%d !(%s)?\n", diminuto_unittest_test, diminuto_unittest_errors, diminuto_unittest_total, #_COND_); \
            if (diminuto_unittest_errno) { \
                diminuto_log_log(DIMINUTO_LOG_PRIORITY_WARNING, DIMINUTO_LOG_HERE "EXPECT: errno=%d=\"%s\".\n", diminuto_unittest_errno, strerror(diminuto_unittest_errno)); \
            } \
        } \
    } while (0)

/**
 * @def ASSERT(_COND_)
 * Emit a warning message if the specified condition @a _COND_ is not true,
 * increment the error counter, and exit immediately.
 */
#define ASSERT(_COND_) \
    do { \
        diminuto_unittest_errno = errno; \
        if (!(_COND_)) { \
            ++diminuto_unittest_errors; \
            ++diminuto_unittest_total; \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, DIMINUTO_LOG_HERE "ASSERT: test=%d errors=%d total=%d !(%s)!\n", diminuto_unittest_test, diminuto_unittest_errors, diminuto_unittest_total, #_COND_); \
            if (diminuto_unittest_errno) { \
                diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, DIMINUTO_LOG_HERE "ASSERT: errno=%d=\"%s\".\n", diminuto_unittest_errno, strerror(diminuto_unittest_errno)); \
            } \
            fflush(stdout); \
            fflush(stderr); \
            exit(diminuto_unittest_total > 255 ? 255 : diminuto_unittest_total); \
        } \
    } while (0)

#endif
