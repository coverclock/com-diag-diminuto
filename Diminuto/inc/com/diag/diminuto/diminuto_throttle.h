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
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"

typedef struct DiminutoThrottle {
    diminuto_ticks_t then;
    diminuto_ticks_t now;
    diminuto_ticks_t increment;     /* GCRA i */
    diminuto_ticks_t limit;         /* GCRA l */
    diminuto_ticks_t expected;      /* GCRA x */
    diminuto_ticks_t actual;        /* GCRA x1 */
    int alarmed;
    int alarming;
} diminuto_throttle_t;

extern diminuto_throttle_t * diminuto_throttle_init(diminuto_throttle_t * throttlep, diminuto_ticks_t increment, diminuto_ticks_t limit);

extern diminuto_throttle_t * diminuto_throttle_reset(diminuto_throttle_t * throttlep);

static inline diminuto_ticks_t diminuto_throttle_now(void)
{
    return diminuto_time_elapsed();
}

extern diminuto_ticks_t diminuto_throttle_request(diminuto_throttle_t * throttlep, diminuto_ticks_t now);

extern int diminuto_throttle_commit(diminuto_throttle_t * throttlep);

#endif
