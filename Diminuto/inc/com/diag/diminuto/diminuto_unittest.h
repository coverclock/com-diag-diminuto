/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_UNITTEST_
#define _H_COM_DIAG_DIMINUTO_UNITTEST_

/**
 * @file
 * @copyright Copyright 2009-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides a simple unit test framework.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * The Unit Test feature is a dirt simple unit testing framework for C
 * programs. If you are using C++ (or even if you are using C but are
 * comfortable with C++) you would be better off using one of the
 * established C++ unit test frameworks. I particularly like Google
 * Test (a.k.a. gtest). Also, developers I know and trust have made good
 * use of CxxUnit, CppUnit, and CxxUnitLite. If you are using Java,
 * go directly to JUnit.
 *
 * The condition must be evaluated exactly once and only once, since
 * it may contain intended side effects.
 *
 * These macros use the GCC-specific _COUNTER_ feature. This can look
 * confusing in a multi-threaded or multi-process unit test (of which
 * there are), since the test number is generated at compile-time,
 * but the number of executed tests is generated at run-time.
 *
 * It's important to capture the errno value right after testing for
 * the condition, since the condition argument will typically either
 * contain the function that sets the errno, or test the return from
 * that function. Still, there is always the chance that the condition
 * coded by the user somehow steps on the errno value from the thing
 * they are trying to test.
 *
 * Judicious choices of using CHECKPOINT versus COMMENT can yield
 * more flexibility in controlling the output from a unit test. It
 * can also really speed them up: a lot of the slowdown in the
 * execution of unit tests comes from latency in the display of
 * messages to the terminal. (Redirecting the output to a file can
 * make a big difference too, but the speed of, for example, SD
 * cards on the Raspberry Pi is nothing to write home about.)
 *
 * COMMENT logs at the DEBUG level.
 *
 * CHECKPOINT logs at the INFORMATION level.
 *
 * TEST, ADVISE, STATUS, and EXIT log at the NOTICE level.
 *
 * FAILURE and EXPECT log at the WARNING level.
 *
 * FATAL, PANIC, and ASSERT log at the ERROR level.
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>

/*******************************************************************************
 * Updates to these variables are serialized by a POSIX mutex.
 ******************************************************************************/

/**
 * Total number of tests executed.
 */
extern int diminuto_unittest_tests;

/**
 * Number of errors in current test.
 */
extern int diminuto_unittest_errors;

/*******************************************************************************
 * Constants.
 ******************************************************************************/

/**
 * Maximum possible exit code (for processes).
 */
static const int DIMINUTO_UNITTEST_MAXIMUM = 255;

/*******************************************************************************
 * Helper functions..
 ******************************************************************************/

/**
 * Set the log mask from the environment and do other useful initialization.
 */
extern void diminuto_unittest_setlogmask(void);

/**
 * Advance the test number and return its new value.
 * @return the new test number.
 */
extern int diminuto_unittest_test(void);

/**
 * Adance the error counter and return its new value.
 * @return the new error counter.
 */
extern int diminuto_unittest_error(void);

/**
 * Flush the standard output and (if necessary) standard error file descriptors.
 */
extern void diminuto_unittest_flush(void);

/*******************************************************************************
 * These macros take no arguments.
 ******************************************************************************/

/**
 * @def SETLOGMASK
 * Set the log mask from the string value of the environmental variable
 * DIMINUTO_LOG_MASK. The value of the environmental variable can be in decimal,
 * hexadecimal, or octal in strtoul() format. Also performs other useful
 * initialization for unit tests, like making sure the standard error stream
 * is unbuffered, and enabling core dumps.
 */
#define SETLOGMASK() \
    diminuto_unittest_setlogmask()

/*******************************************************************************
 * These macros each take a single string constant as an argument.
 ******************************************************************************/

/**
 * @def TEST
 * Emit a notice message identifying the the start of a new unit test
 * and an optional @a _TITLE_ constant string e.g. TEST("Thread Safety").
 */
#define TEST(_TITLE_) \
    do { \
        int diminuto_unittest_number = 0; \
        diminuto_unittest_number = diminuto_unittest_test(); \
        DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "TEST: %d tests=%d " _TITLE_ "\n", (__COUNTER__), diminuto_unittest_number); \
        diminuto_unittest_flush(); \
    } while (0)

/**
 * @def STATUS
 * Emit a count of the errors so far and an optional @a _TITLE_ constant
 * string e.g. STATUS("So far, so good").
 */
#define STATUS(_TITLE_) \
    do { \
        int diminuto_unittest_number; \
        int diminuto_unittest_count; \
        diminuto_unittest_number = diminuto_unittest_tests; \
        diminuto_unittest_count = diminuto_unittest_errors; \
        DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "STATUS: tests=%d errors=%d %s " _TITLE_ "\n", diminuto_unittest_number, diminuto_unittest_count, (diminuto_unittest_errors == 0) ? "SUCCESS." : "FAILURE!"); \
        diminuto_unittest_flush(); \
    } while (0)

/**
 * @def FAILURE
 * Increment the error counter and emit a warning message and an optional
 * @a _TITLE_ constant string argument e.g. FAILURE("That could have gone better.").
 */
#define FAILURE(_TITLE_) \
    do { \
        int diminuto_unittest_number; \
        int diminuto_unittest_count; \
        diminuto_unittest_number = diminuto_unittest_tests; \
        diminuto_unittest_count = diminuto_unittest_error(); \
        DIMINUTO_LOG_WARNING(DIMINUTO_LOG_HERE "FAILURE: tests=%d errors=%d %s " _TITLE_ "\n", diminuto_unittest_number, diminuto_unittest_count, "FAILURE!"); \
    } while (0)

/**
 * @def EXIT
 * Emit  a notice message with an optional @a _TITLE_ string argument
 * e.g. EXIT("That's all, folks!"). Exit the calling process with a
 * zero exit code if there are no errors, or a non-zero exit code
 * if there are.
 */
#define EXIT(_TITLE_) \
    do { \
        int diminuto_unittest_number; \
        int diminuto_unittest_count; \
        diminuto_unittest_number = diminuto_unittest_tests; \
        diminuto_unittest_count = diminuto_unittest_errors; \
        DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "EXIT: tests=%d errors=%d %s " _TITLE_ "\n", diminuto_unittest_number, diminuto_unittest_count, (diminuto_unittest_count == 0) ? "SUCCESS." : "FAILURE!"); \
        diminuto_unittest_flush(); \
        exit(diminuto_unittest_count > DIMINUTO_UNITTEST_MAXIMUM ? DIMINUTO_UNITTEST_MAXIMUM : diminuto_unittest_count); \
    } while (0)

/**
 * @def TEXIT
 * Emit  a notice message with an optional @a _TITLE_ string argument
 * e.g. TEXIT("So long!"). Exit the calling Thread with a zero exit code
 * if there are no errors, or a non-zero exit code if there are. This
 * invokes the native POSIX thread exit function. If you are using the
 * Diminuto thread feature, the Thread object may not be left in the
 * expected state.
 */
#define TEXIT(_TITLE_) \
    do { \
        int diminuto_unittest_number; \
        int diminuto_unittest_count; \
        diminuto_unittest_number = diminuto_unittest_tests; \
        diminuto_unittest_count = diminuto_unittest_errors; \
        DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "TEXIT: tests=%d errors=%d %s " _TITLE_ "\n", diminuto_unittest_number, diminuto_unittest_count, (diminuto_unittest_count == 0) ? "SUCCESS." : "FAILURE!"); \
        diminuto_unittest_flush(); \
        pthread_exit((void *)(intptr_t)(diminuto_unittest_count > DIMINUTO_UNITTEST_MAXIMUM ? DIMINUTO_UNITTEST_MAXIMUM : diminuto_unittest_count)); \
    } while (0)

/**
 * @def FATAL
 * Emit a error message and an optional @a _TITLE_ string argument e.g.
 * FATAL("Farewell, cruel world!"). Exit the calling process with a
 * non-zero exit code.
 */
#define FATAL(_TITLE_) \
    do { \
        int diminuto_unittest_errno; \
        int diminuto_unittest_number; \
        int diminuto_unittest_count; \
        diminuto_unittest_errno = errno; \
        diminuto_unittest_number = diminuto_unittest_tests; \
        diminuto_unittest_count = diminuto_unittest_error(); \
        DIMINUTO_LOG_ERROR(DIMINUTO_LOG_HERE "FATAL: tests=%d errors=%d errno=%d %s " _TITLE_ "\n", diminuto_unittest_number, diminuto_unittest_count, diminuto_unittest_errno, "FAILURE!"); \
        diminuto_unittest_flush(); \
        exit(diminuto_unittest_count > DIMINUTO_UNITTEST_MAXIMUM ? DIMINUTO_UNITTEST_MAXIMUM : diminuto_unittest_count); \
    } while (0)

/**
 * @def PANIC
 * Emit a error message and an optional @a _TITLE_ string argument e.g.
 * PANIC("Aaaaaaaaahhhhhhhh!"). Try to dump core.
 */
#define PANIC(_TITLE_) \
    do { \
        int diminuto_unittest_errno; \
        int diminuto_unittest_number; \
        int diminuto_unittest_count; \
        diminuto_unittest_errno = errno; \
        diminuto_unittest_number = diminuto_unittest_tests; \
        diminuto_unittest_count = diminuto_unittest_error(); \
        DIMINUTO_LOG_ERROR(DIMINUTO_LOG_HERE "PANIC: tests=%d errors=%d errno=%d %s " _TITLE_ "\n", diminuto_unittest_number, diminuto_unittest_count, diminuto_unittest_errno, "PANIC!"); \
        diminuto_unittest_flush(); \
        diminuto_core_enable(); \
        diminuto_core_fatal(); \
        exit(diminuto_unittest_count > DIMINUTO_UNITTEST_MAXIMUM ? DIMINUTO_UNITTEST_MAXIMUM : diminuto_unittest_count); \
    } while (0)

/*******************************************************************************
 * These macros each take an expression that evaluated once yield 0 or !0.
 ******************************************************************************/

/**
 * @def ADVISE
 * Emit a notice message if the specified condition @a _CONDITION_ is not true.
 * We emit the errno number even thoough it might not be germane.
 */
#define ADVISE(_CONDITION_) \
    do { \
        int diminuto_unittest_condition; \
        diminuto_unittest_condition = !!(_CONDITION_); \
        if (!diminuto_unittest_condition) { \
            int diminuto_unittest_errno; \
            int diminuto_unittest_number; \
            int diminuto_unittest_count; \
            diminuto_unittest_errno = errno; \
            diminuto_unittest_number = diminuto_unittest_tests; \
            diminuto_unittest_count = diminuto_unittest_errors; \
            DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "ADVISE: tests=%d errors=%d errno=%d !(%s).\n", diminuto_unittest_number, diminuto_unittest_count, diminuto_unittest_errno, #_CONDITION_); \
            diminuto_unittest_flush(); \
        } \
    } while (0)

/**
 * @def EXPECT
 * Emit a warning message if the specified condition @a _CONDITION_ is not true
 * and increment the error counter.
 * We emit the errno number even thoough it might not be germane.
 */
#define EXPECT(_CONDITION_) \
    do { \
        int diminuto_unittest_condition; \
        diminuto_unittest_condition = !!(_CONDITION_); \
        if (!diminuto_unittest_condition) { \
            int diminuto_unittest_errno; \
            int diminuto_unittest_number; \
            int diminuto_unittest_count; \
            diminuto_unittest_errno = errno; \
            diminuto_unittest_number = diminuto_unittest_tests; \
            diminuto_unittest_count = diminuto_unittest_error(); \
            DIMINUTO_LOG_WARNING(DIMINUTO_LOG_HERE "EXPECT: tests=%d errors=%d errno=%d !(%s)?\n", diminuto_unittest_number, diminuto_unittest_count, diminuto_unittest_errno, #_CONDITION_); \
            diminuto_unittest_flush(); \
        } \
    } while (0)

/**
 * @def ASSERT
 * Emit a warning message if the specified condition @a _CONDITION_ is not true,
 * increment the error counter, and exit immediately.
 * We emit the errno number even thoough it might not be germane.
 */
#define ASSERT(_CONDITION_) \
    do { \
        int diminuto_unittest_condition; \
        diminuto_unittest_condition = !!(_CONDITION_); \
        if (!diminuto_unittest_condition) { \
            int diminuto_unittest_errno; \
            int diminuto_unittest_number; \
            int diminuto_unittest_count; \
            diminuto_unittest_errno = errno; \
            diminuto_unittest_number = diminuto_unittest_tests; \
            diminuto_unittest_count = diminuto_unittest_error(); \
            DIMINUTO_LOG_ERROR(DIMINUTO_LOG_HERE "ASSERT: tests=%d errors=%d errno=%d !(%s)!\n", diminuto_unittest_number, diminuto_unittest_count, diminuto_unittest_errno,  #_CONDITION_); \
            diminuto_unittest_flush(); \
            exit(diminuto_unittest_count > DIMINUTO_UNITTEST_MAXIMUM ? DIMINUTO_UNITTEST_MAXIMUM : diminuto_unittest_count); \
        } \
    } while (0)

/*******************************************************************************
 * These macros each take zero or more printf-style format and argument lists.
 ******************************************************************************/

/**
 * @def CHECKPOINT
 * Emit a notice message with the current translation unit file and line nunber.
 */
#define CHECKPOINT(...) \
    DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "CHECKPOINT: " __VA_ARGS__); \

/**
 * @def COMMENT
 * Emit a debug message with the current translation unit file and line number.
 */
#define COMMENT(...) \
    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "COMMENT: " __VA_ARGS__); \

#endif
