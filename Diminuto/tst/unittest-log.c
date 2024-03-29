/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Log feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Log feature. Because the Unit Test framework
 * uses the Log feature, this unit test is not built on top of the Unit Test
 * framework.
 *
 * It's useful to run this as
 *
 *	setsid unittest-log
 *
 * or
 *
 *	( ( unittest-log & ) & ) &
 *
 * which exercises the code that detects whether or not it's a deamon
 * by its session identifier or who its parent process is.
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_environment.h"
#include "com/diag/diminuto/diminuto_error.h"
#include "com/diag/diminuto/diminuto_fs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "unittest-log.h"

int main(int argc, char ** argv)
{
    {
        fputs("TEST 0\n", stderr); fflush(stderr);
        assert(diminuto_log_priority2mask(DIMINUTO_LOG_PRIORITY_EMERGENCY)   == DIMINUTO_LOG_MASK_EMERGENCY);
        assert(diminuto_log_priority2mask(DIMINUTO_LOG_PRIORITY_ALERT)       == DIMINUTO_LOG_MASK_ALERT);
        assert(diminuto_log_priority2mask(DIMINUTO_LOG_PRIORITY_CRITICAL)    == DIMINUTO_LOG_MASK_CRITICAL);
        assert(diminuto_log_priority2mask(DIMINUTO_LOG_PRIORITY_ERROR)       == DIMINUTO_LOG_MASK_ERROR);
        assert(diminuto_log_priority2mask(DIMINUTO_LOG_PRIORITY_WARNING)     == DIMINUTO_LOG_MASK_WARNING);
        assert(diminuto_log_priority2mask(DIMINUTO_LOG_PRIORITY_NOTICE)      == DIMINUTO_LOG_MASK_NOTICE);
        assert(diminuto_log_priority2mask(DIMINUTO_LOG_PRIORITY_INFORMATION) == DIMINUTO_LOG_MASK_INFORMATION);
        assert(diminuto_log_priority2mask(DIMINUTO_LOG_PRIORITY_DEBUG)       == DIMINUTO_LOG_MASK_DEBUG);
        assert(diminuto_log_priority2mask((diminuto_log_priority_t)(-1))     == DIMINUTO_LOG_MASK_ANY);
    }

    {
        diminuto_log_mask = DIMINUTO_LOG_MASK_NONE;
        fputs("TEST 1\n", stderr); fflush(stderr);
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ALERT));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ERROR));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_WARNING));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NOTICE));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_DEBUG));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ANY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NONE));

        diminuto_log_mask = DIMINUTO_LOG_MASK_EMERGENCY;
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ALERT));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ERROR));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_WARNING));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NOTICE));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_DEBUG));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ANY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NONE));

        diminuto_log_mask = DIMINUTO_LOG_MASK_ALERT;
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ALERT));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ERROR));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_WARNING));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NOTICE));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_DEBUG));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ANY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NONE));

        diminuto_log_mask = DIMINUTO_LOG_MASK_CRITICAL;
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ALERT));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ERROR));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_WARNING));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NOTICE));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_DEBUG));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ANY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NONE));

        diminuto_log_mask = DIMINUTO_LOG_MASK_ERROR;
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ALERT));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_CRITICAL));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ERROR));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_WARNING));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NOTICE));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_DEBUG));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ANY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NONE));

        diminuto_log_mask = DIMINUTO_LOG_MASK_WARNING;
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ALERT));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ERROR));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_WARNING));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NOTICE));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_DEBUG));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ANY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NONE));

        diminuto_log_mask = DIMINUTO_LOG_MASK_NOTICE;
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ALERT));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ERROR));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_WARNING));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NOTICE));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_DEBUG));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ANY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NONE));

        diminuto_log_mask = DIMINUTO_LOG_MASK_INFORMATION;
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ALERT));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ERROR));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_WARNING));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NOTICE));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_DEBUG));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ANY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NONE));

        diminuto_log_mask = DIMINUTO_LOG_MASK_DEBUG;
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ALERT));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ERROR));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_WARNING));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NOTICE));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_INFORMATION));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_DEBUG));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ANY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NONE));

        diminuto_log_mask = DIMINUTO_LOG_MASK_ALL;
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ALERT));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_CRITICAL));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ERROR));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_WARNING));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NOTICE));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_INFORMATION));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_DEBUG));
        assert(diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_ANY));
        assert(!diminuto_log_mask_isenabled(DIMINUTO_LOG_MASK_NONE));
    }

    {
        diminuto_log_mask = DIMINUTO_LOG_MASK_NONE;
        fputs("TEST 2\n", stderr); fflush(stderr);
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_EMERGENCY));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ALERT));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_CRITICAL));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ERROR));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_WARNING));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_NOTICE));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_INFORMATION));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_DEBUG));

        diminuto_log_mask = DIMINUTO_LOG_MASK_EMERGENCY;
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_EMERGENCY));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ALERT));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_CRITICAL));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ERROR));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_WARNING));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_NOTICE));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_INFORMATION));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_DEBUG));

        diminuto_log_mask = DIMINUTO_LOG_MASK_ALERT;
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_EMERGENCY));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ALERT));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_CRITICAL));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ERROR));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_WARNING));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_NOTICE));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_INFORMATION));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_DEBUG));

        diminuto_log_mask = DIMINUTO_LOG_MASK_CRITICAL;
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_EMERGENCY));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ALERT));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_CRITICAL));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ERROR));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_WARNING));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_NOTICE));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_INFORMATION));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_DEBUG));

        diminuto_log_mask = DIMINUTO_LOG_MASK_ERROR;
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_EMERGENCY));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ALERT));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_CRITICAL));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ERROR));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_WARNING));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_NOTICE));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_INFORMATION));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_DEBUG));

        diminuto_log_mask = DIMINUTO_LOG_MASK_WARNING;
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_EMERGENCY));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ALERT));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_CRITICAL));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ERROR));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_WARNING));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_NOTICE));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_INFORMATION));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_DEBUG));

        diminuto_log_mask = DIMINUTO_LOG_MASK_NOTICE;
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_EMERGENCY));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ALERT));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_CRITICAL));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ERROR));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_WARNING));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_NOTICE));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_INFORMATION));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_DEBUG));

        diminuto_log_mask = DIMINUTO_LOG_MASK_INFORMATION;
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_EMERGENCY));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ALERT));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_CRITICAL));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ERROR));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_WARNING));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_NOTICE));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_INFORMATION));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_DEBUG));

        diminuto_log_mask = DIMINUTO_LOG_MASK_DEBUG;
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_EMERGENCY));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ALERT));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_CRITICAL));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ERROR));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_WARNING));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_NOTICE));
        assert(!diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_INFORMATION));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_DEBUG));

        diminuto_log_mask = DIMINUTO_LOG_MASK_ALL;
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_EMERGENCY));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ALERT));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_CRITICAL));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_ERROR));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_WARNING));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_NOTICE));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_INFORMATION));
        assert(diminuto_log_priority_isenabled(DIMINUTO_LOG_PRIORITY_DEBUG));
    }

    {
        fputs("TEST 3\n", stderr); fflush(stderr);

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_EMERGENCY;
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALERT;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_CRITICAL;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ERROR;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_WARNING;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NOTICE;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_INFORMATION;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_DEBUG;
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(!DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALL;
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_EMERGENCY));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ALERT));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_CRITICAL));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_ERROR));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_WARNING));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_NOTICE));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_INFORMATION));
        assert(DIMINUTO_LOG_ENABLED(DIMINUTO_LOG_MASK_DEBUG));

        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_DEFAULT;
    }

    {
        diminuto_log_mask_t prior;

        fputs("TEST 4\n", stderr); fflush(stderr);

        diminuto_log_mask = 0xa5;

        prior = diminuto_log_initmask(0);
        assert(prior == 0xa5);
        assert(diminuto_log_mask == 0);

        prior = diminuto_log_initmask(0xfe);
        assert(prior == 0);
        assert(diminuto_log_mask == 0xfe);

        prior = diminuto_log_initmask(DIMINUTO_LOG_MASK_ALL);
        assert(prior == 0xfe);
        assert(diminuto_log_mask == 0xff);

        prior = diminuto_log_initmask(DIMINUTO_LOG_MASK_DEFAULT);
        assert(prior == DIMINUTO_LOG_MASK_ALL);
        assert(diminuto_log_mask == 0xfc);
    }


    {
        fputs("TEST 5\n", stderr); fflush(stderr);

        diminuto_log_mask = 0xa5;

        assert(diminuto_setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "0", !0) == 0);
        diminuto_log_setmask();
        assert(diminuto_log_mask == 0);

        assert(diminuto_setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "255", !0) == 0);
        diminuto_log_setmask();
        assert(diminuto_log_mask == 255);

        assert(diminuto_setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "0377", !0) == 0);
        diminuto_log_setmask();
        assert(diminuto_log_mask == 0377);

        assert(diminuto_setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "0xff", !0) == 0);
        diminuto_log_setmask();
        assert(diminuto_log_mask == 0xff);

        assert(diminuto_setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, "~0", !0) == 0);
        diminuto_log_setmask();
        assert(diminuto_log_mask == DIMINUTO_LOG_MASK_ALL);

        {
            diminuto_log_mask_t mask;
            char buffer[5];

            fputs("TEST 5.1\n", stderr);

            for (mask = 0; mask < 256; ++mask) {

                fprintf(stderr, "TEST 5.1.%d\n", mask);

                (void)snprintf(buffer, sizeof(buffer), "%u", mask);
                assert(diminuto_setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, buffer, !0) == 0);
                diminuto_log_setmask();
                assert(diminuto_log_mask == mask);

                (void)snprintf(buffer, sizeof(buffer), "0x%x", mask);
                assert(diminuto_setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, buffer, !0) == 0);
                diminuto_log_setmask();
                assert(diminuto_log_mask == mask);

                (void)snprintf(buffer, sizeof(buffer), "0%o", mask);
                assert(diminuto_setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, buffer, !0) == 0);
                diminuto_log_setmask();
                assert(diminuto_log_mask == mask);
            }
        }

        {
            static const char NO_SUCH_FILE[] = "no_such_file";
            diminuto_log_mask_t mask;

            fputs("TEST 5.2\n", stderr);

            assert(diminuto_fs_type(diminuto_log_mask_path) == DIMINUTO_FS_TYPE_FILE);
            diminuto_log_mask = 0xa5;
            mask = diminuto_log_importmask(diminuto_log_mask_path);
            assert(mask == 0xff);
            assert(diminuto_log_mask == mask);

            assert(diminuto_fs_type(NO_SUCH_FILE) == DIMINUTO_FS_TYPE_NONE);
            diminuto_log_mask = 0xa5;
            mask = diminuto_log_importmask(NO_SUCH_FILE);
            assert(mask == 0xa5);
            assert(diminuto_log_mask == mask);

            diminuto_log_mask = 0xa5;
            mask = diminuto_log_importmask(NULL);
            /* We don't know what this will be, but we shouldn't core dump. */
        }

        {
            char buffer[5];
            static const diminuto_log_priority_t PRIORITY[] = {
                DIMINUTO_LOG_PRIORITY_EMERGENCY,
                DIMINUTO_LOG_PRIORITY_ALERT,
                DIMINUTO_LOG_PRIORITY_CRITICAL,
                DIMINUTO_LOG_PRIORITY_ERROR,
                DIMINUTO_LOG_PRIORITY_WARNING,
                DIMINUTO_LOG_PRIORITY_NOTICE,
                DIMINUTO_LOG_PRIORITY_INFORMATION,
                DIMINUTO_LOG_PRIORITY_DEBUG,
            };
            diminuto_log_priority_t priority;
            diminuto_log_mask_t mask;
            int ii;

            fputs("TEST 5.3\n", stderr);

            for (ii = 0; ii < countof(PRIORITY); ++ii) {
                fprintf(stderr, "TEST 5.3.%d\n", ii);
                priority = PRIORITY[ii];
                mask = diminuto_log_priority2mask(priority);
                (void)snprintf(buffer, sizeof(buffer), "%u", mask);
                assert(diminuto_setenv(DIMINUTO_LOG_MASK_NAME_DEFAULT, buffer, !0) == 0);
                diminuto_log_setmask();
                assert(diminuto_log_mask == mask);
                assert(diminuto_log_mask_isenabled(mask));
                assert(diminuto_log_priority_isenabled(priority));
            }
        }

        diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;

    }

    {
        char buffer[DIMINUTO_LOG_BUFFER_MAXIMUM];
        int ii;

        fputs("TEST 6\n", stderr); fflush(stderr);

        assert(diminuto_log_priority == DIMINUTO_LOG_PRIORITY_DEFAULT);

        diminuto_log_log(diminuto_log_priority, DIMINUTO_LOG_HERE "HERE");
        for (ii = 0; ii < sizeof(buffer) - 1; ++ii) {
            buffer[ii] = (ii % ('~' - ' ' + 1)) + ' ';
        }
        buffer[sizeof(buffer) - 1] = '\0';
        diminuto_log_log(diminuto_log_priority, "%s%s%s", DIMINUTO_LOG_HERE, "THERE", buffer);

    }

    {
        FILE * fp;
        int fd;

        fputs("TEST 7\n", stderr); fflush(stderr);

        assert(diminuto_log_descriptor == STDERR_FILENO);
        assert(diminuto_log_file == (FILE *)0);

        diminuto_log_emit("1: Logging\n");
        fprintf(stderr, "1: Printing\n");
        write(STDERR_FILENO, "1: Writing\n", 11);

        fp = diminuto_log_stream();
        assert(fp != (FILE *)0);
        assert(fp == diminuto_log_file);
        assert(fileno(fp) == fileno(diminuto_log_file));
        assert(fp == stderr);
        assert(fileno(fp) == fileno(stderr));

        assert(diminuto_log_descriptor == STDERR_FILENO);
        assert(diminuto_log_file == stderr);
        assert(fileno(diminuto_log_file) == fileno(stderr));
        assert(fileno(diminuto_log_file) == STDERR_FILENO);

        diminuto_log_emit("2: Logging\n");
        fprintf(diminuto_log_file, "2: Printing\n");
        write(diminuto_log_descriptor, "2: Writing\n", 11);

        diminuto_log_descriptor = STDOUT_FILENO;
        fp = diminuto_log_stream();
        assert(fp != (FILE *)0);
        assert(fp == diminuto_log_file);
        assert(fileno(fp) == fileno(diminuto_log_file));
        assert(fp == stdout);
        assert(fileno(fp) == fileno(stdout));

        assert(diminuto_log_descriptor == STDOUT_FILENO);
        assert(diminuto_log_file == stdout);
        assert(fileno(diminuto_log_file) == fileno(stdout));
        assert(fileno(diminuto_log_file) == STDOUT_FILENO);

        diminuto_log_emit("3: Logging\n");
        fprintf(diminuto_log_file, "3: Printing\n");
        fflush(diminuto_log_file); /* Buffered. */
        write(diminuto_log_descriptor, "3: Writing\n", 11);

        fd = dup(STDOUT_FILENO);
        assert(fd != STDOUT_FILENO);
        assert(fd != STDERR_FILENO);
        diminuto_log_descriptor = fd;
        fp = diminuto_log_stream(); /* Will fdopen(3). */
        assert(fp != (FILE *)0);
        assert(fp == diminuto_log_file);
        assert(fp != stdout);
        assert(fp != stderr);
        assert(fileno(fp) == fileno(diminuto_log_file));
        assert(fileno(fp) != STDOUT_FILENO);
        assert(fileno(fp) != STDERR_FILENO);

        assert(diminuto_log_descriptor == fd);
        assert(diminuto_log_file != stdout);
        assert(fileno(diminuto_log_file) != fileno(stdout));
        assert(fileno(diminuto_log_file) != STDOUT_FILENO);

        diminuto_log_emit("4: Logging\n");
        fprintf(diminuto_log_file, "4: Printing\n");
        write(diminuto_log_descriptor, "4: Writing\n", 11);

        diminuto_log_descriptor = STDERR_FILENO;
        fp = diminuto_log_stream(); /* Will fdclose(3). */
        assert(fp != (FILE *)0);
        assert(fp == diminuto_log_file);
        assert(fileno(fp) == fileno(diminuto_log_file));
        assert(fp == stderr);
        assert(fileno(fp) == fileno(stderr));

        assert(diminuto_log_descriptor == STDERR_FILENO);
        assert(diminuto_log_file == stderr);
        assert(fileno(diminuto_log_file) == fileno(stderr));
        assert(fileno(diminuto_log_file) == STDERR_FILENO);

        diminuto_log_emit("5: Logging\n");
        fprintf(diminuto_log_file, "5: Printing\n");
        write(diminuto_log_descriptor, "5: Writing\n", 11);

    }

    fputs("TEST 8\n", stderr); fflush(stderr);

    if (argc < 2) {
        /* Do nothing. */
    } else if (strcmp(argv[1], "daemon") == 0) {
        int rc;
        fprintf(stderr, DIMINUTO_LOG_HERE "DAEMONIZING\n");
        if ((rc = diminuto_daemon(argv[0])) == 0) {
            fprintf(stderr, DIMINUTO_LOG_HERE "DAEMONIZED\n");
        } else {
            diminuto_perror("diminuto_daemon");
            exit(1);
        }
    } else if (strcmp(argv[1], "suppress") == 0) {
        fprintf(stderr, DIMINUTO_LOG_HERE "SUPPRESSED\n");
        diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_SUPPRESS;
    } else if (strcmp(argv[1], "automatic") == 0) {
        fprintf(stderr, DIMINUTO_LOG_HERE "AUTOMATIC\n");
        diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_AUTOMATIC;
    } else if (strcmp(argv[1], "stderr") == 0) {
        fprintf(stderr, DIMINUTO_LOG_HERE "STDERR\n");
        diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_STDERR;
    } else if (strcmp(argv[1], "syslog") == 0) {
        fprintf(stderr, DIMINUTO_LOG_HERE "SYSLOG\n");
        diminuto_log_strategy = DIMINUTO_LOG_STRATEGY_SYSLOG;
    } else {
        fprintf(stderr, "usage: %s [ automatic | daemon | stderr | suppress | syslog ]\n", argv[0]);
        exit(1);
    }

    {
        fputs("TEST 9\n", stderr); fflush(stderr);

        diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;
        fprintf(stderr, DIMINUTO_LOG_HERE "BEGIN\n");
        yes();
        no();
        maybe();
        fprintf(stderr, DIMINUTO_LOG_HERE "PUBLIC DEFAULT\n");
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PUBLIC ALL\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_ALL;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PUBLIC EMERGENCY\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_EMERGENCY;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PUBLIC ALERT\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_ALERT;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PUBLIC CRITICAL\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_CRITICAL;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PUBLIC ERROR\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_ERROR;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PUBLIC WARNING\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_WARNING;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PUBLIC NOTICE\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_NOTICE;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PUBLIC INFORMATION\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_INFORMATION;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PUBLIC DEBUG\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_DEBUG;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PUBLIC NONE\n");
        diminuto_log_mask = DIMINUTO_LOG_MASK_NONE;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE DEFAULT\n");
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE ALL\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALL;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE NONE\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE EMERGENCY\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_EMERGENCY;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE ALERT\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ALERT;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE CRITICAL\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_CRITICAL;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE ERROR\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_ERROR;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE WARNING\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_WARNING;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE NOTICE\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NOTICE;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE INFORMATION\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_INFORMATION;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE DEBUG\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_DEBUG;
        all();
        none();
        mine();
        fprintf(stderr, DIMINUTO_LOG_HERE "PRIVATE NONE\n");
        diminuto_log_subsystem[0] = DIMINUTO_LOG_MASK_NONE;
        all();
        none();
        mine();
        diminuto_log_mask = DIMINUTO_LOG_MASK_DEFAULT;

    }

    {
        const static int ERRNO[] = { E2BIG, EACCES, EADDRINUSE, EAGAIN, EBADF, EBUSY, ECHILD, ECONNREFUSED, EEXIST, EINVAL, };
        char buffer[sizeof("ERRNO[XXXXXXXXXX] (xxxxxxxxxxxxxxxx)")];
        int ii;

        fputs("TEST 10\n", stderr); fflush(stderr);

        fprintf(stderr, DIMINUTO_LOG_HERE "ERRNO PERROR default\n");

        for (ii = 0; ii < countof(ERRNO); ++ii) {
            snprintf(buffer, sizeof(buffer), "ERRNO[%d] (default)", ERRNO[ii]);
            errno = ERRNO[ii];
            diminuto_perror(buffer);
        }

        fprintf(stderr, DIMINUTO_LOG_HERE "ERRNO SERROR default\n");

        for (ii = 0; ii < countof(ERRNO); ++ii) {
            snprintf(buffer, sizeof(buffer), "ERRNO[%d] (default)", ERRNO[ii]);
            errno = ERRNO[ii];
            diminuto_serror(buffer);
        }

    }

    {
        const static int ERRNO[] = { E2BIG, EACCES, EADDRINUSE, EAGAIN, EBADF, EBUSY, ECHILD, ECONNREFUSED, EEXIST, EINVAL, };
        char buffer[sizeof("ERRNO[XXXXXXXXXX] (xxxxxxxxxxxxxxxx)")];
        int ii;

        fputs("TEST 11\n", stderr); fflush(stderr);

        diminuto_log_error_suppress = true;

        fprintf(stderr, DIMINUTO_LOG_HERE "ERRNO PERROR suppressed\n");

        for (ii = 0; ii < countof(ERRNO); ++ii) {
            snprintf(buffer, sizeof(buffer), "ERRNO[%d] (suppressed)", ERRNO[ii]);
            errno = ERRNO[ii];
            diminuto_perror(buffer);
        }

        fprintf(stderr, DIMINUTO_LOG_HERE "ERRNO SERROR suppressed\n");

        for (ii = 0; ii < countof(ERRNO); ++ii) {
            snprintf(buffer, sizeof(buffer), "ERRNO[%d] (suppressed)", ERRNO[ii]);
            errno = ERRNO[ii];
            diminuto_serror(buffer);
        }

    }

    {
        const static int ERRNO[] = { E2BIG, EACCES, EADDRINUSE, EAGAIN, EBADF, EBUSY, ECHILD, ECONNREFUSED, EEXIST, EINVAL, };
        char buffer[sizeof("ERRNO[XXXXXXXXXX] (xxxxxxxxxxxxxxxx)")];
        int ii;

        fputs("TEST 12\n", stderr); fflush(stderr);

        diminuto_log_error_suppress = false;

        fprintf(stderr, DIMINUTO_LOG_HERE "ERRNO PERROR enabled\n");

        for (ii = 0; ii < countof(ERRNO); ++ii) {
            snprintf(buffer, sizeof(buffer), "ERRNO[%d] (enabled)", ERRNO[ii]);
            errno = ERRNO[ii];
            diminuto_perror(buffer);
        }

        fprintf(stderr, DIMINUTO_LOG_HERE "ERRNO SERROR enabled\n");

        for (ii = 0; ii < countof(ERRNO); ++ii) {
            snprintf(buffer, sizeof(buffer), "ERRNO[%d] (enabled)", ERRNO[ii]);
            errno = ERRNO[ii];
            diminuto_serror(buffer);
        }

    }

    {
        char hostname[DIMINUTO_LOG_HOSTNAME_MAXIMUM];

        fputs("TEST 13\n", stderr); fflush(stderr);

        assert(sizeof(hostname) >= sizeof("localhost"));

    }

    fputs("TEST 14\n", stderr); fflush(stderr);

    assert(diminuto_log_lost == 0);

    fprintf(stderr, "%s[%d]: SUCCESS.\n", __FILE__, __LINE__);

    exit(0);
}
