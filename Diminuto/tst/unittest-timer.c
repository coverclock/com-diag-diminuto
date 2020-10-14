/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include <signal.h>

static void * isr(void * vp) { return (void *)0; }

int main(int argc, char ** argv)
{
	SETLOGMASK();
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, 0, (diminuto_timer_function_t *)0, 0) == (diminuto_timer_t *)0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, 0, isr, SIGALRM) == (diminuto_timer_t *)0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, 0, isr, 0) == &timer);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_init_generic(&timer, 0, isr, 0) == &timer);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, 0, (diminuto_timer_function_t *)0, SIGALRM) == &timer);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_init_generic(&timer, 0, (diminuto_timer_function_t *)0, SIGALRM) == &timer);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, !0, (diminuto_timer_function_t *)0, 0) == (diminuto_timer_t *)0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, !0, isr, SIGALRM) == (diminuto_timer_t *)0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, !0, isr, 0) == &timer);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_init_generic(&timer, !0, isr, 0) == &timer);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, !0, (diminuto_timer_function_t *)0, SIGALRM) == &timer);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_init_generic(&timer, !0, (diminuto_timer_function_t *)0, SIGALRM) == &timer);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        STATUS();
    }
    EXIT();
}
