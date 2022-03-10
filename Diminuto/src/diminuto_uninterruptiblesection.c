/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Uninterruptible Section feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Uninterruptible Section feature.
 */

#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>

int diminuto_uninterruptiblesection_block(const int signals[], size_t count, sigset_t * sp)
{
    int rc = 0;
    int ii = 0;
    sigset_t set;

    if ((rc = sigemptyset(&set)) < 0) {
        diminuto_perror("diminuto_uninterruptiblesection_block: sigemptyset");
    } else {
        for (ii = 0; ii < count; ++ii) {
            if ((rc = sigaddset(&set, signals[ii])) < 0) {
                break;
            }
        }
        if (rc < 0) {
            diminuto_perror("diminuto_uninterruptiblesection_block: sigaddset");
        } else if ((rc = pthread_sigmask(SIG_BLOCK, &set, sp)) != 0) {
            errno = rc;
            diminuto_perror("diminuto_uninterruptiblesection_block: pthread_sigmask");
            rc = -1;
        } else {
            /* Do nothing. */
        }
    }

    return rc;
}

void diminuto_uninterruptiblesection_cleanup(void * vp)
{
    sigset_t * sp = (sigset_t *)vp;
    int rc = 0;

    if ((rc = pthread_sigmask(SIG_SETMASK, sp, (sigset_t *)0)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_uninterruptiblesection_cleanup: pthread_sigmask");
    }
}
