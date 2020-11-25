/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2016-2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Unit Test feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Unit Test feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_core.h"
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int diminuto_unittest_tests = 0;
int diminuto_unittest_errors = 0;

void diminuto_unittest_setlogmask()
{
    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        diminuto_log_setmask();
        setvbuf(stderr, (char *)0, _IONBF, 0);
        diminuto_core_enable();
    DIMINUTO_CRITICAL_SECTION_END;
}

int diminuto_unittest_test()
{
    int tests = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        tests = ++diminuto_unittest_tests;
    DIMINUTO_CRITICAL_SECTION_END;

    return tests;
}

int diminuto_unittest_error()
{
    int errors = -1; 

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        errors = ++diminuto_unittest_errors;
    DIMINUTO_CRITICAL_SECTION_END;

    return errors;
}

void diminuto_unittest_flush()
{
    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        fflush(stdout);
        fflush(stderr);
    DIMINUTO_CRITICAL_SECTION_END;
}
