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
    int rc = -1;
    int datum = 0;

    abort();
    diminuto_perror("diminuto_core_fatal: abort");

    rc = kill(getpid(), SIGABRT);
    diminuto_perror("diminuto_core_fatal: kill(SIGABRT)");

    rc = kill(getpid(), SIGKILL);
    diminuto_perror("diminuto_core_fatal: kill(SIGKILL)");

    /*
     * Remarkably, I have worked on hardware where deferencing
     * a null pointer in C succeeds.
     */
    datum = *((volatile int *)0);
    errno = EINVAL;
    diminuto_perror("diminuto_core_fatal: *NULL");

    rc = kill(getpid(), SIGSEGV);
    diminuto_perror("diminuto_core_fatal: kill(SIGSEGV)");

    _exit(1);
}
