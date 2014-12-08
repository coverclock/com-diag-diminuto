/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_THROTTLE_
#define _H_COM_DIAG_DIMINUTO_THROTTLE_

/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * WORK IN PROGRESS!
 *
 * Implement a Generic Cell Rate Algorithm (GCRA) using a virtual scheduler.
 *
 * REFERENCES
 *
 * ATM Forum, Traffic Management Specification Version 4.1, af-tm-0121.000,
 * March 1999
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"

typedef struct DiminutoThrottle {
    diminuto_ticks_t now;
    diminuto_ticks_t then;
    diminuto_ticks_t increment;     /* GCRA i */
    diminuto_ticks_t limit;         /* GCRA l */
    diminuto_ticks_t expected;      /* GCRA x */
    diminuto_ticks_t actual;        /* GCRA x1 */
    unsigned int alarmed : 1;
    unsigned int alarming: 1;
} diminuto_throttle_t;

static inline diminuto_throttle_t * diminuto_throttle_reset(diminuto_throttle_t * throttlep, diminuto_ticks_t now)
{
    throttlep->now = now;
    throttlep->then = now - throttlep->increment;
    throttlep->expected = throttlep->increment;
    throttlep->actual = 0;
    throttlep->alarmed = 0;
    throttlep->alarming = 0;
    return throttlep;
}

static inline diminuto_throttle_t * diminuto_throttle_init(diminuto_throttle_t * throttlep, diminuto_ticks_t increment, diminuto_ticks_t limit, diminuto_ticks_t now)
{
    throttlep->increment = increment;
    throttlep->limit = limit;
    return diminuto_throttle_reset(throttlep, now);
}

static inline diminuto_ticks_t diminuto_throttle_now(void)
{
    return diminuto_time_elapsed();
}

extern diminuto_ticks_t diminuto_throttle_request(diminuto_throttle_t * throttlep, diminuto_ticks_t now);

extern int diminuto_throttle_commitn(diminuto_throttle_t * throttlep, size_t events);

static inline int diminuto_throttle_commit(diminuto_throttle_t * throttlep)
{
    return diminuto_throttle_commitn(throttlep, 1);
}

int diminuto_throttle_admitn(diminuto_throttle_t * throttlep, diminuto_ticks_t now, size_t events);

static inline int diminuto_throttle_admit(diminuto_throttle_t * throttlep, diminuto_ticks_t now)
{
    return diminuto_throttle_admitn(throttlep, now, 1);
}

#endif
