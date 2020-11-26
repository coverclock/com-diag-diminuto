/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2008-2010 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Core feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Core feature.
 */

#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

int diminuto_core_enable(void)
{
    int rc = -1;
    struct rlimit limit;

    do {

        if (getrlimit(RLIMIT_CORE, &limit) < 0) {
            diminuto_perror("diminuto_core_enable: getrlimit");
            break;
        }

        limit.rlim_cur = limit.rlim_max;

        if (setrlimit(RLIMIT_CORE, &limit) < 0) {
            diminuto_perror("diminuto_core_enable: setrlimit");
            break;
        }

        rc = 0;

    } while (0);

    return rc;
}

void diminuto_core_fatal(void)
{

    abort();
    diminuto_perror("diminuto_core_fatal: abort");

    kill(0, SIGABRT);
    diminuto_perror("diminuto_core_fatal: kill(SIGABRT)");

    kill(0, SIGKILL);
    diminuto_perror("diminuto_core_fatal: kill(SIGKILL)");

    kill(0, SIGSEGV);
    diminuto_perror("diminuto_core_fatal: kill(SIGSEGV)");

    /*
     * Remarkably, I have worked on embedded systems where
     * deferencing a null pointer in C succeeds. We put a
     * special value in that memory location, e.g.
     * 0xdeadbeef, so that we would recognize it if it
     * showed up in a log message, a core dump, or a stack
     * trace.
     */

    *((volatile int *)0) = 0;
    errno = EINVAL;
    diminuto_perror("diminuto_core_fatal: *NULL");

    _exit(255);
    diminuto_perror("diminuto_core_fatal: _exit(1)");

    /*
     * THOU SHALT NOT PASS!
     */

    while (!0) { diminuto_yield(); }
}
