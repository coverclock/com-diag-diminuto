/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_UNITTEST_
#define _H_COM_DIAG_DIMINUTO_UNITTEST_

/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Colorado, USA<BR>
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
#include "com/diag/diminuto/diminuto_token.h"
#include <stdlib.h>
#include <stdio.h>

static int diminuto_unittest_errors = 0;

#define CHECKPOINT(...) \
    diminuto_log_log(LOG_NOTICE, __FILE__ "@" DIMINUTO_TOKEN_TOKEN(__LINE__) ": " __VA_ARGS__)

/**
 * @def EXIT()
 * Exit the calling process with a zero exit code if there are no errors,
 * or a non-zero exit code if there are.
 */
#define EXIT() \
	do { \
		fflush(stdout); \
		fflush(stderr); \
		exit(diminuto_unittest_errors > 255 ? 255 : diminuto_unittest_errors); \
	} while (0)

/**
 * @def EXPECT(_COND_)
 * Log a warning message if the specified condition @a _COND_ is not true
 * and increment the error counter.
 */
#define EXPECT(_COND_) \
    do { \
        if (!(_COND_)) { \
            diminuto_log_log(LOG_WARNING, "%s@%d: !EXPECT(" #_COND_ ")!\n", __FILE__, __LINE__); \
            ++diminuto_unittest_errors; \
        } \
    } while (0)


/**
 * @def ASSERT(_COND_)
 * Log a warning message if the specified condition @a _COND_ is not true,
 * increment the error counter, and exit immediately.
 */
#define ASSERT(_COND_) \
    do { \
        if (!(_COND_)) { \
            diminuto_log_log(LOG_ERR, "%s@%d: !ASSERT(" #_COND_ ")!\n", __FILE__, __LINE__); \
            ++diminuto_unittest_errors; \
            EXIT(); \
        } \
    } while (0)

#define FATAL(...) \
	    do { \
	        diminuto_log_log(LOG_ERR, __FILE__ "@" DIMINUTO_TOKEN_TOKEN(__LINE__) ": " __VA_ARGS__); \
	        ++diminuto_unittest_errors; \
	        EXIT(); \
	    } while (0)

#endif
