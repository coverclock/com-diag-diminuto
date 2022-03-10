/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2017-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Critical Section feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Critical Section feature.
 */

#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>

int diminuto_criticalsection_lock(pthread_mutex_t * mp)
{
    int rc = 0;

    if ((rc = pthread_mutex_lock(mp)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_criticalsection_lock: pthread_mutex_lock");
        rc = -1;
    }

    return rc;
}

int diminuto_criticalsection_trylock(pthread_mutex_t * mp)
{
    int rc = 0;

    if ((rc = pthread_mutex_trylock(mp)) == EBUSY) {
        errno = rc;
        rc = -1;
    } else if (rc != 0) {
        errno = rc;
        diminuto_perror("diminuto_criticalsection_trylock: pthread_mutex_trylock");
        rc = -1;
    } else {
        /* Do nothing. */
    }

    return rc;
}

void diminuto_criticalsection_cleanup(void * vp)
{
    int rc = 0;
    pthread_mutex_t * mutexp = (pthread_mutex_t *)vp;

    if ((rc = pthread_mutex_unlock(mutexp)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_criticalsection_cleanup: pthread_mutex_unlock");
    }
}
