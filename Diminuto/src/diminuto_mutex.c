/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

/***********************************************************************
 * PREREQUISITES
 **********************************************************************/

#include <errno.h>
#include "com/diag/diminuto/diminuto_mutex.h"
#include "com/diag/diminuto/diminuto_log.h"

/***********************************************************************
 * INITIALIZERS AND FINALIZERS
 **********************************************************************/

diminuto_mutex_t * diminuto_mutex_init(diminuto_mutex_t * mp)
{
    diminuto_mutex_t * result = (diminuto_mutex_t *)0;
    int rc = EIO;

    if ((rc = pthread_mutexattr_init(&(mp->attribute))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_init: pthread_mutexattr_init");
    } else if ((rc = pthread_mutexattr_settype(&(mp->attribute), PTHREAD_MUTEX_RECURSIVE)) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_init: pthread_mutexattr_settype");
    } else if ((rc = pthread_mutex_init(&(mp->mutex), &(mp->attribute))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_init: pthread_mutex_init");
    } else {
        result = mp;
    }

    return result;
}

diminuto_mutex_t * diminuto_mutex_fini(diminuto_mutex_t * mp)
{
    diminuto_mutex_t * result = mp;
    int rc = EIO;

    if ((rc = pthread_mutex_destroy(&(mp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_fini: pthread_mutex_destroy");
    } else if ((rc = pthread_mutexattr_destroy(&(mp->attribute))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_fini: pthread_mutexattr_destroy");
    } else {
        result = (diminuto_mutex_t *)0;
    }

    return result;
}

/***********************************************************************
 * OPERATIONS
 **********************************************************************/

int diminuto_mutex_lock(diminuto_mutex_t * mp)
{
    int rc = EIO;

    if ((rc = pthread_mutex_lock(&(mp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_lock: pthread_mutex_lock");
    }

    return rc;
}

int diminuto_mutex_lock_try(diminuto_mutex_t * mp)
{
    int rc = EIO;

    if ((rc = pthread_mutex_trylock(&(mp->mutex))) == 0) {
        /* Do nothing. */ 
    } else if (rc == DIMINUTO_MUTEX_BUSY) {
        /* Do nothing. */ 
    } else {
        errno = rc;
        diminuto_perror("diminuto_mutex_lock_try: pthread_mutex_trylock");
    }

    return rc;
}

int diminuto_mutex_unlock(diminuto_mutex_t * mp)
{
    int rc = EIO;

    if ((rc = pthread_mutex_unlock(&(mp->mutex))) != 0) {
        errno = rc;
        diminuto_perror("diminuto_mutex_unlock: pthread_mutex_unlock");
    }

    return rc;
}

/***********************************************************************
 * CALLBACKS
 **********************************************************************/

void diminuto_mutex_cleanup(void * vp)
{
    diminuto_mutex_t * mp = (diminuto_mutex_t *)vp;

    diminuto_mutex_unlock(mp);
}
