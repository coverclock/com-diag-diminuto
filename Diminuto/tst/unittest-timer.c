/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Timer feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Timer feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "../src/diminuto_timer.h"
#include <signal.h>

static void * callback(void * vp) { return (void *)0; }

int main(int argc, char ** argv)
{
	SETLOGMASK();
    {
        TEST();
        ASSERT(diminuto_timer_window(0) == diminuto_frequency());
        ASSERT(diminuto_timer_window(1) == diminuto_frequency());
        ASSERT(diminuto_timer_window(2) == diminuto_frequency());
        ASSERT(diminuto_timer_window(diminuto_frequency()) == (2 * (diminuto_frequency())));
        ASSERT(diminuto_timer_window(diminuto_frequency() + 1) == (2 * (diminuto_frequency() + 1)));
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, 0, (diminuto_timer_function_t *)0, 0) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_error(&timer) == 0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, 0, callback, SIGALRM) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_error(&timer) == 0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, 0, callback, 0) == &timer);
        ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
        ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_init_generic(&timer, 0, callback, 0) == &timer);
        ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
        ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, 0, (diminuto_timer_function_t *)0, SIGALRM) == &timer);
        ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
        ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_init_generic(&timer, 0, (diminuto_timer_function_t *)0, SIGALRM) == &timer);
        ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
        ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, !0, (diminuto_timer_function_t *)0, 0) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_error(&timer) == 0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, !0, callback, SIGALRM) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
        ASSERT(diminuto_timer_error(&timer) == 0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, !0, callback, 0) == &timer);
        ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
        ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_init_generic(&timer, !0, callback, 0) == &timer);
        ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
        ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        STATUS();
    }
    {
        diminuto_timer_t timer;
        TEST();
        ASSERT(diminuto_timer_init_generic(&timer, !0, (diminuto_timer_function_t *)0, SIGALRM) == &timer);
        ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
        ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        ASSERT(diminuto_timer_init_generic(&timer, !0, (diminuto_timer_function_t *)0, SIGALRM) == &timer);
        ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
        ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);
        STATUS();
    }
    EXIT();
}
