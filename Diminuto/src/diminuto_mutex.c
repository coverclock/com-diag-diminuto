/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <errno.h>
#include "com/diag/diminuto/diminuto_mutex.h"
#include "com/diag/diminuto/diminuto_log.h"

diminuto_mutex_t * diminuto_mutex_init(diminuto_mutex_t * mp)
{
    pthread_mutexattr_init(&(mp->attribute));
    pthread_mutexattr_settype(&(mp->attribute), PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&(mp->mutex), &(mp->attribute));
    return mp;
}

diminuto_mutex_t * diminuto_mutex_fini(diminuto_mutex_t * mp)
{
    pthread_mutex_destroy(&(mp->mutex));
    pthread_mutexattr_destroy(&(mp->attribute));
    return (diminuto_mutex_t *)0;
}

static void diminuto_mutex_cleanup(void * vp)
{
    int rc = EIO;
    diminuto_mutex_t * mp = (diminuto_mutex_t *)vp;

    if ((rc = pthread_mutex_unlock(&(mp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_cleanup: pthread_mutex_unlock");
    }
}

int diminuto_mutex_begin(diminuto_mutex_t * mp)
{
    int rc = EIO;

    pthread_cleanup_push(diminuto_mutex_cleanup, (void *)mp);
    if ((rc = pthread_mutex_lock(&(mp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_begin: pthread_mutex_lock"); }
        pthread_cleanup_pop(0);
    }

    return rc;
}

int diminuto_mutex_begin_try(diminuto_mutex_t * mp)
{
    int rc = EIO;

    pthread_cleanup_push(diminuto_mutex_cleanup, (void *)mp);
    if ((rc = pthread_mutex_trylock(&(mp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_begin_try: pthread_mutex_trylock"); }
        pthread_cleanup_pop(0);
    }

    return rc;
}

int diminuto_mutex_end(diminuto_mutex_t * mp)
{
    int rc = EIO;

    if ((rc = pthread_mutex_unlock(&(this->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_end: pthread_mutex_unlock"); }
    } else {
        pthread_cleanup_pop(0);
    }

    return rc;
}
