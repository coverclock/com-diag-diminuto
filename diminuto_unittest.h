/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_UNITTEST_
#define _H_COM_DIAG_DIMINUTO_UNITTEST_

/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * If you are using C++ you would be better off using one of the
 * established C++ unit test frameworks. I particularly like CxxTest
 * and Google Test. But developers I know and trust have made good
 * use of CppUnit. (If you are using Java, go directly to JUnit.)
 */

#include "diminuto_log.h"
#include <stdlib.h>

static int errors = 0;

#define EXIT() \
    exit(errors > 255 ? 255 : errors)

#define EXPECT(_COND_) \
    do { \
        if (!(_COND_)) { \
            diminuto_log(LOG_WARNING, \
                "%s@%d: !EXPECT(" #_COND_ ")!\n", __FILE__, __LINE__); \
            ++errors; \
        } \
    } while (0)

#define ASSERT(_COND_) \
    do { \
        if (!(_COND_)) { \
            diminuto_log(LOG_ERR, \
                "%s@%d: !ASSERT(" #_COND_ ")!\n", __FILE__, __LINE__); \
            ++errors; \
            EXIT(); \
        } \
    } while (0)

#endif
