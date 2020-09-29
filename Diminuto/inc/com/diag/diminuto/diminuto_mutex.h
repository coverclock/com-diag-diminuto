/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MUTEX_
#define _H_COM_DIAG_DIMINUTO_MUTEX_

/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#define __USE_GNU
#define _GNU_SOURCE
#include <pthread.h>
#include <errno.h>

static const int DIMINUTO_MUTEX_BUSY = EBUSY;

typedef struct DiminutoMutex {
    pthread_mutex_t mutex;
    pthread_mutexattr_t attribute;
} diminuto_mutex_t;

#define DIMINUTO_MUTEX_INITIALIZER \
    { \
        PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP, \
    }

extern diminuto_mutex_t * diminuto_mutex_init(diminuto_mutex_t * mp);

extern int diminuto_mutex_lock(diminuto_mutex_t * mp);

extern int diminuto_mutex_lock_try(diminuto_mutex_t * mp);

extern int diminuto_mutex_unlock(diminuto_mutex_t * mp);

extern void diminuto_mutex_cleanup(void * vp);

extern diminuto_mutex_t * diminuto_mutex_fini(diminuto_mutex_t * mp);

#endif
