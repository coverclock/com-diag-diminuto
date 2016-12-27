/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_UNITTEST_
#define _H_COM_DIAG_DIMINUTO_UNITTEST_

/**
 * @file
 *
 * Copyright 2009-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This is a dirt simple unit testing framework for C programs.
 * If you are using C++ (or even if you are using C but are
 * comfortable with C++) you would be better off using one of the
 * established C++ unit test frameworks. I particularly like Google
 * Test (a.k.a. gtest). But developers I know and trust have made good
 * use of CxxUnit, CppUnit, and CxxUnitLite. (If you are using Java,
 * go directly to JUnit.)
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include <stdlib.h>
#include <stdio.h>

extern int diminuto_unittest_tests;
extern int diminuto_unittest_errors;

/**
 * @def SETLOGMASK()
 * Set the log mask from the string value of the environmental variable
 * DIMINUTO_LOG_MASK. The value of the environmental variable can be in decimal,
 * hexadecimal, or octal in strtoul() format.
 */
#define SETLOGMASK() \
    diminuto_log_setmask()

/**
 * @def TEST()
 * Emit a notice message identifying the the start of a new unit test.
 */
#define TEST() \
    diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE "TEST(): test=%d\n", diminuto_unittest_tests++)

/**
 * @def CHECKPOINT(...)
 * Emit a notice message with the current translation unit file and line number
 * and an optional manifest string argument.
 */
#define CHECKPOINT(...) \
    diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE __VA_ARGS__)

/**
 * @def COMMENT(...)
 * Emit a debug message with the current translation unit file and line number
 * and an optional manifest string argument.
 */
#define COMMENT(...) \
    diminuto_log_log(DIMINUTO_LOG_PRIORITY_DEBUG, DIMINUTO_LOG_HERE __VA_ARGS__)

/**
 * @def EXIT()
 * Exit the calling process with a zero exit code if there are no errors,
 * or a non-zero exit code if there are.
 */
#define EXIT() \
    do { \
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE "EXIT(): tests=%d errors=%d %s\n", diminuto_unittest_tests, diminuto_unittest_errors, (diminuto_unittest_errors == 0) ? "SUCCESS." : "FAILURE!"); \
        fflush(stdout); \
        fflush(stderr); \
        exit(diminuto_unittest_errors > 255 ? 255 : diminuto_unittest_errors); \
    } while (0)

/**
 * @def STATUS()
 * Report errors so far.
 */
#define STATUS() \
    do { \
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE "STATUS(): test=%d errors=%d %s\n", diminuto_unittest_tests, diminuto_unittest_errors, (diminuto_unittest_errors == 0) ? "SUCCESS." : "FAILURE!"); \
        fflush(stdout); \
        fflush(stderr); \
    } while (0)

/**
 * @def ADVISE(_COND_)
 * Log a notice message if the specified condition @a _COND_ is not true.
 */
#define ADVISE(_COND_) \
    do { \
        if (!(_COND_)) { \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_NOTICE, DIMINUTO_LOG_HERE "!ADVISE(" #_COND_ ")!\n"); \
        } \
    } while (0)

/**
 * @def EXPECT(_COND_)
 * Log a warning message if the specified condition @a _COND_ is not true
 * and increment the error counter.
 */
#define EXPECT(_COND_) \
    do { \
        if (!(_COND_)) { \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_WARNING, DIMINUTO_LOG_HERE "!EXPECT(" #_COND_ ")!\n"); \
            ++diminuto_unittest_errors; \
        } \
    } while (0)

/**
 * @def FAILURE()
 * Log a warning message and increment the error counter.
 */
#define FAILURE() \
    do { \
        diminuto_log_log(DIMINUTO_LOG_PRIORITY_WARNING, DIMINUTO_LOG_HERE "FAILURE!\n"); \
        ++diminuto_unittest_errors; \
    } while (0)

/**
 * @def ASSERT(_COND_)
 * Log a warning message if the specified condition @a _COND_ is not true,
 * increment the error counter, and exit immediately.
 */
#define ASSERT(_COND_) \
    do { \
        if (!(_COND_)) { \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, DIMINUTO_LOG_HERE "!ASSERT(" #_COND_ ")!\n"); \
            ++diminuto_unittest_errors; \
            EXIT(); \
        } \
    } while (0)

/**
 * @def FATAL(...)
 * Emit a error message and exit the calling process with a non-zero exit code.
 */
#define FATAL(...) \
        do { \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, DIMINUTO_LOG_HERE __VA_ARGS__); \
            ++diminuto_unittest_errors; \
            EXIT(); \
	    } while (0)

/**
 * @def PANIC(...)
 * Emit a error message and try to dump core.
 */
#define PANIC(...) \
        do { \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, DIMINUTO_LOG_HERE __VA_ARGS__); \
            fflush(stdout); \
            fflush(stderr); \
            diminuto_core_enable(); \
            FATAL("0x%x", *((volatile char *)0)); \
            diminuto_log_log(DIMINUTO_LOG_PRIORITY_ERROR, DIMINUTO_LOG_HERE __VA_ARGS__); \
            EXIT(); \
        } while (0)

#endif
