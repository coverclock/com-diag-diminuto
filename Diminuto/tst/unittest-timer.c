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
#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "../src/diminuto_timer.h"
#include <signal.h>

static void * callback(void * vp) { return (void *)0; }

int main(int argc, char ** argv)
{
	SETLOGMASK();

    {
        TEST();

        ASSERT((int)DIMINUTO_MODULATOR_STATE_IDLE == (int)DIMINUTO_TIMER_STATE_IDLE);
        ASSERT((int)DIMINUTO_MODULATOR_STATE_ARM == (int)DIMINUTO_TIMER_STATE_ARM);
        ASSERT((int)DIMINUTO_MODULATOR_STATE_DISARM == (int)DIMINUTO_TIMER_STATE_DISARM);
        ASSERT((int)DIMINUTO_MODULATOR_STATE_UNKNOWN == (int)DIMINUTO_TIMER_STATE_UNKNOWN);

        STATUS();
    }

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
        diminuto_timer_t timer = DIMINUTO_TIMER_INITIALIZER((diminuto_timer_function_t *)0);

        TEST();

        ASSERT(timer.function == (diminuto_timer_function_t *)0);
        ASSERT(diminuto_timer_error(&timer) == 0);

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

        /*
         * VALGRIND ISSUE: this block of test code - and only this block -
         * is failing valgrind, which complains about a "possible" memory
         * leak. After poking around in my own code for a long time, I finally
         * Googled it, to discover I am hardly the only person to have seen
         * this. The current hypothesis is that the POSIX thread code allocates
         * memory on the first call, and then caches it for later use. This
         * block is the first successful Diminuto Timer initilization in this
         * unit test. Weird.
         */

        ASSERT(diminuto_timer_init_generic(&timer, 0, callback, 0) == &timer);
            ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
            ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);

        STATUS();
    }

    {
        diminuto_timer_t timer;
        diminuto_timer_t timer2;

        TEST();

        ASSERT(diminuto_timer_init_generic(&timer, 0, callback, 0) == &timer);
            ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
            ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);

        ASSERT(diminuto_timer_init_generic(&timer, 0, callback, 0) == &timer);
            ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
            ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);

        ASSERT(diminuto_timer_init_generic(&timer, 0, callback, 0) == &timer);
            ASSERT(diminuto_timer_state(&timer) == DIMINUTO_TIMER_STATE_IDLE);
            ASSERT(diminuto_timer_error(&timer) == 0);
        ASSERT(diminuto_timer_fini(&timer) == (diminuto_timer_t *)0);

        ASSERT(diminuto_timer_init_generic(&timer2, 0, callback, 0) == &timer2);
            ASSERT(diminuto_timer_state(&timer2) == DIMINUTO_TIMER_STATE_IDLE);
            ASSERT(diminuto_timer_error(&timer2) == 0);
        ASSERT(diminuto_timer_fini(&timer2) == (diminuto_timer_t *)0);

        ASSERT(diminuto_timer_init_generic(&timer2, 0, callback, 0) == &timer2);
            ASSERT(diminuto_timer_state(&timer2) == DIMINUTO_TIMER_STATE_IDLE);
            ASSERT(diminuto_timer_error(&timer2) == 0);
        ASSERT(diminuto_timer_fini(&timer2) == (diminuto_timer_t *)0);

        ASSERT(diminuto_timer_init_generic(&timer2, 0, callback, 0) == &timer2);
            ASSERT(diminuto_timer_state(&timer2) == DIMINUTO_TIMER_STATE_IDLE);
            ASSERT(diminuto_timer_error(&timer2) == 0);
        ASSERT(diminuto_timer_fini(&timer2) == (diminuto_timer_t *)0);

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
