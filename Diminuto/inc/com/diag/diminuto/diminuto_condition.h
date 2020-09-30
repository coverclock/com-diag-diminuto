/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CONDITION_
#define _H_COM_DIAG_DIMINUTO_CONDITION_

/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

/***********************************************************************
 *
 **********************************************************************/

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_mutex.h"

/***********************************************************************
 *
 **********************************************************************/

static const diminuto_ticks_t DIMINUTO_CONDITION_INFINITY = ~(diminuto_ticks_t)0;

static const int DIMINUTO_CONDITION_TIMEDOUT = ETIMEDOUT;

/***********************************************************************
 *
 **********************************************************************/

typedef struct DiminutoCondition {
    diminuto_mutex_t mutex;             /* Diminuto mutual exclusion object */
    pthread_cond_t condition;           /* POSIX Thread condition object */
} diminuto_condition_t;

#define DIMINUTO_CONDITION_INITIALIZER \
    { \
        DIMINUTO_MUTEX_INITIALIZER, \
        PTHREAD_COND_INITIALIZER, \
    }

/***********************************************************************
 *
 **********************************************************************/

extern diminuto_condition_t * diminuto_condition_init(diminuto_condition_t * cp);

extern diminuto_condition_t * diminuto_condition_fini(diminuto_condition_t * cp);

static inline int diminuto_condition_lock(diminuto_condition_t * cp)
{
    return diminuto_mutex_lock(&(cp->mutex));
}

static inline int diminuto_condition_lock_try(diminuto_condition_t * cp)
{
    return diminuto_mutex_lock_try(&(cp->mutex));
}

static inline int diminuto_condition_unlock(diminuto_condition_t * cp)
{
    return diminuto_mutex_unlock(&(cp->mutex));
}

extern void diminuto_condition_cleanup(void * vp);

/**
 * Return the system clock time in Coordinated Universal Time (UTC) in
 * ticks since the Epoch (shown here in ISO8601 format)
 * 1970-01-01T00:00:00+0000.
 * @return the number of ticks elapsed since the Epoch or -1 with
 * errno set if an error occurred.
 */
static inline diminuto_sticks_t diminuto_condition_clock(void)
{
    return diminuto_time_clock();
}

extern int diminuto_condition_wait_until(diminuto_condition_t * cp, diminuto_ticks_t clocktime);

static inline int diminuto_condition_wait(diminuto_condition_t * cp)
{
    return diminuto_condition_wait_until(cp, DIMINUTO_CONDITION_INFINITY);
}

extern int diminuto_condition_signal(diminuto_condition_t * cp);

/***********************************************************************
 *
 **********************************************************************/

#define DIMINUTO_CONDITION_BEGIN(_CP_) DIMINUTO_MUTEX_BEGIN(&((_CP_)->mutex))

#define DIMINUTO_CONDITION_TRY(_CP_) DIMINUTO_MUTEX_TRY(&((_CP_)->mutex))

#define DIMINUTO_CONDITION_END DIMINUTO_MUTEX_END

/***********************************************************************
 *
 **********************************************************************/

#endif
