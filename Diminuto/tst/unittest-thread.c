/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Thread feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Thread feature.
 */

#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_policy.h"
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

static int shared = 0;
static const int LIMIT = 100;

static void * body1(void * arg)
{
    static diminuto_mutex_t mutex = DIMINUTO_MUTEX_INITIALIZER;
    int done = 0;

    ASSERT(diminuto_thread_instance() != (diminuto_thread_t *)0);

    ASSERT(diminuto_thread_state(diminuto_thread_instance()) == DIMINUTO_THREAD_STATE_RUNNING);

    while (!done) {

        ASSERT(diminuto_mutex_lock(&mutex) == 0);

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
            } else {
                /* Do nothing. */
            }

        ASSERT(diminuto_mutex_unlock(&mutex) == 0);

        diminuto_thread_yield();

    }

    ASSERT(diminuto_thread_state(diminuto_thread_instance()) == DIMINUTO_THREAD_STATE_RUNNING);

    return arg;
}

static void * body2(void * arg)
{
    static diminuto_mutex_t mutex = DIMINUTO_MUTEX_INITIALIZER;
    int done = 0;
    int rc = -1;
    int busy = 0;

    ASSERT(diminuto_thread_instance() != (diminuto_thread_t *)0);

    ASSERT(diminuto_thread_state(diminuto_thread_instance()) == DIMINUTO_THREAD_STATE_RUNNING);

    while (!done) {

        rc = diminuto_mutex_lock_try(&mutex);
        ASSERT((rc == 0) || (rc == DIMINUTO_MUTEX_BUSY));
        if (rc == 0) {

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
            } else {
                /* Do nothing. */
            }

            ASSERT(diminuto_mutex_unlock(&mutex) == 0);
        } else {
            COMMENT("BUSY");
            busy += 1;
        }

        diminuto_thread_yield();

    }

    ADVISE(busy > 0);

    ASSERT(diminuto_thread_state(diminuto_thread_instance()) == DIMINUTO_THREAD_STATE_RUNNING);

    return arg;
}

static void * body3(void * arg)
{
    static diminuto_mutex_t mutex = DIMINUTO_MUTEX_INITIALIZER;
    int done = 0;

    ASSERT(diminuto_thread_instance() != (diminuto_thread_t *)0);

    ASSERT(diminuto_thread_state(diminuto_thread_instance()) == DIMINUTO_THREAD_STATE_RUNNING);

    while (!done) {

        DIMINUTO_MUTEX_BEGIN(&mutex);

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
            } else {
                /* Do nothing. */
            }

        DIMINUTO_MUTEX_END;

        diminuto_thread_yield();

    }

    ASSERT(diminuto_thread_state(diminuto_thread_instance()) == DIMINUTO_THREAD_STATE_RUNNING);

    return arg;
}

static void * body4(void * arg)
{
    static diminuto_mutex_t mutex = DIMINUTO_MUTEX_INITIALIZER;
    int done = 0;
    int successful = 0;
    int busy = 0;

    ASSERT(diminuto_thread_instance() != (diminuto_thread_t *)0);

    ASSERT(diminuto_thread_state(diminuto_thread_instance()) == DIMINUTO_THREAD_STATE_RUNNING);

    while (!done) {

        successful = 0;

        DIMINUTO_MUTEX_TRY(&mutex);

            successful = !0;

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
            } else {
                /* Do nothing. */
            }

        DIMINUTO_MUTEX_END;

        if (!successful) {
            COMMENT("BUSY");
            busy += 1;
        }

        diminuto_thread_yield();

    }

    ADVISE(busy > 0);

    ASSERT(diminuto_thread_state(diminuto_thread_instance()) == DIMINUTO_THREAD_STATE_RUNNING);

    return arg;
}

static void * body5(void * arg)
{
    diminuto_thread_t * tp;
    diminuto_ticks_t now;
    tp = diminuto_thread_instance();
    ASSERT(diminuto_thread_state(tp) == DIMINUTO_THREAD_STATE_RUNNING);
    DIMINUTO_THREAD_BEGIN(tp);
        while (diminuto_thread_notifications() == 0) {
            now = diminuto_thread_clock() + diminuto_frequency();
            diminuto_thread_wait_until(tp, now);
        }
    DIMINUTO_THREAD_END;
    COMMENT("NOTIFIED");
    ASSERT(diminuto_thread_state(tp) == DIMINUTO_THREAD_STATE_RUNNING);
    return arg;
}

static void * body6(void * arg)
{
    diminuto_thread_t * tp;
    uintptr_t limit;
    uintptr_t total;
    uintptr_t index;
    tp = diminuto_thread_instance();
    ASSERT(diminuto_thread_state(tp) == DIMINUTO_THREAD_STATE_RUNNING);
    limit = (uintptr_t)arg;
    DIMINUTO_THREAD_BEGIN(tp);
        for (total = diminuto_thread_notifications(); total < limit; total += diminuto_thread_notifications()) {
            COMMENT("WAITING");
            diminuto_thread_wait(tp);
            for (index = total; index > 0; --index) {
                COMMENT("YIELDING");
                diminuto_thread_yield();
            }
        }
    DIMINUTO_THREAD_END;
    COMMENT("EXITING");
    ASSERT(diminuto_thread_state(tp) == DIMINUTO_THREAD_STATE_RUNNING);
    diminuto_thread_exit((void *)6);
    return (void *)0;
}

static void * body7(void * arg)
{
    diminuto_thread_t * tp;
    diminuto_mux_t mux;
    int rc;
    tp = diminuto_thread_instance();
    ASSERT(diminuto_thread_state(tp) == DIMINUTO_THREAD_STATE_RUNNING);
    diminuto_mux_init(&mux);
    while (!0) {
        COMMENT("BLOCKING");
        rc = diminuto_mux_wait(&mux, -1);
        if (rc >= 0) {
            COMMENT("READY");
        } else if (errno != EINTR) {
            COMMENT("ERROR");
        } else {
            COMMENT("INTERRUPTED");
            break;
        }
    }
    DIMINUTO_THREAD_BEGIN(tp);
        if (diminuto_thread_notifications()) {
            COMMENT("NOTIFIED");
        }
    DIMINUTO_THREAD_END;
    ASSERT(diminuto_thread_state(tp) == DIMINUTO_THREAD_STATE_RUNNING);
    pthread_exit((void *)7);
    return (void *)0;
}

static void * body8(void * arg)
{
    static diminuto_condition_t condition = DIMINUTO_CONDITION_INITIALIZER;
    int done = 0;

    ASSERT(diminuto_thread_instance() != (diminuto_thread_t *)0);

    ASSERT(diminuto_thread_state(diminuto_thread_instance()) == DIMINUTO_THREAD_STATE_RUNNING);

    while (!done) {

        DIMINUTO_CONDITION_BEGIN(&condition);

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
                diminuto_condition_signal(&condition);
            } else {
                diminuto_condition_wait(&condition);
            }

        DIMINUTO_CONDITION_END;

    }

    ASSERT(diminuto_thread_state(diminuto_thread_instance()) == DIMINUTO_THREAD_STATE_RUNNING);

    return arg;
}

int main(int argc, char ** argv)
{
    diminuto_log_setmask();

    {
        TEST();

        ASSERT(DIMINUTO_THREAD_NOTIFY == SIGUSR1);
        ASSERT(DIMINUTO_THREAD_INFINITY == DIMINUTO_CONDITION_INFINITY);
        ASSERT((int)DIMINUTO_THREAD_ERROR ==  (int)DIMINUTO_CONDITION_ERROR);
        ASSERT((int)DIMINUTO_THREAD_TIMEDOUT == (int)DIMINUTO_CONDITION_TIMEDOUT);

        STATUS();
    }

    {
        TEST();
        COMMENT("main.pthread_self=0x%08lx\n", pthread_self());
        ASSERT(pthread_equal(pthread_self(), diminuto_thread_self()));
        ASSERT(diminuto_thread_yield() == 0);
        ASSERT(diminuto_thread_notifications() == 0);
        STATUS();
    }

    {
        diminuto_thread_t * tp;
        TEST();
        ASSERT((tp = diminuto_thread_instance()) != (diminuto_thread_t *)0);
        ASSERT(pthread_equal(tp->thread, diminuto_thread_self()));
        ASSERT(tp->function == (void * (*)(void *))main);
        ASSERT(tp->context == (void *)0);
        ASSERT(tp->value == (void *)~0);
        ASSERT(tp->state == DIMINUTO_THREAD_STATE_RUNNING);
        ASSERT(tp->notify == 0);
        ASSERT(tp->notifications == 0);
        STATUS();
    }

    {
        diminuto_thread_t thread = DIMINUTO_THREAD_INITIALIZER(body1);
        TEST();
        ASSERT(diminuto_thread_state(&thread) == DIMINUTO_THREAD_STATE_INITIALIZED);
        ASSERT(thread.function == body1);
        ASSERT(thread.context == (void *)0);
        ASSERT(thread.value == (void *)~0);
        ASSERT(thread.state == DIMINUTO_THREAD_STATE_INITIALIZED);
        ASSERT(thread.notify == DIMINUTO_THREAD_NOTIFY);
        ASSERT(thread.notifications == 0);
        STATUS();
    }

    {
        diminuto_thread_t thread = { 0, };
        TEST();
        ASSERT(diminuto_thread_state(&thread) == DIMINUTO_THREAD_STATE_ALLOCATED);
        ASSERT(thread.state == DIMINUTO_THREAD_STATE_ALLOCATED);
        ASSERT(diminuto_thread_init(&thread, body2) == &thread);
        ASSERT(diminuto_thread_state(&thread) == DIMINUTO_THREAD_STATE_INITIALIZED);
        ASSERT(thread.function == body2);
        ASSERT(thread.context == (void *)0);
        ASSERT(thread.value == (void *)~0);
        ASSERT(thread.state == DIMINUTO_THREAD_STATE_INITIALIZED);
        ASSERT(thread.notify == DIMINUTO_THREAD_NOTIFY);
        ASSERT(thread.notifications == 0);
        ASSERT(diminuto_thread_fini(&thread) == (diminuto_thread_t *)0);
        ASSERT(thread.state == DIMINUTO_THREAD_STATE_FINALIZED);
        ASSERT(diminuto_thread_state(&thread) == DIMINUTO_THREAD_STATE_FINALIZED);
        STATUS();
    }

    {
        int rc;
        diminuto_thread_t odd;
        diminuto_thread_t even;
        void * final;

        TEST();

        shared = 0;

        ASSERT(diminuto_thread_init(&odd, body1) == &odd);
        ASSERT(diminuto_thread_state(&odd) == DIMINUTO_THREAD_STATE_INITIALIZED);
        ASSERT(diminuto_thread_init(&even, body1) == &even);
        ASSERT(diminuto_thread_state(&even) == DIMINUTO_THREAD_STATE_INITIALIZED);

        rc = diminuto_thread_start(&odd, (void *)1);
        ASSERT(rc == 0);

        rc = diminuto_thread_start(&even, (void *)0);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = diminuto_thread_join(&odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);
        ASSERT(diminuto_thread_state(&odd) == DIMINUTO_THREAD_STATE_JOINED);

        final = (void *)~0;
        rc = diminuto_thread_join(&even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);
        ASSERT(diminuto_thread_state(&even) == DIMINUTO_THREAD_STATE_JOINED);

        ASSERT(shared == LIMIT);

        ASSERT(diminuto_thread_fini(&even) == (diminuto_thread_t *)0);
        ASSERT(diminuto_thread_state(&even) == DIMINUTO_THREAD_STATE_FINALIZED);
        ASSERT(diminuto_thread_fini(&odd) == (diminuto_thread_t *)0);
        ASSERT(diminuto_thread_state(&odd) == DIMINUTO_THREAD_STATE_FINALIZED);

        STATUS();
    }

    {
        int rc;
        diminuto_thread_t odd = DIMINUTO_THREAD_INITIALIZER(body2);
        diminuto_thread_t even = DIMINUTO_THREAD_INITIALIZER(body2);
        void * final;

        TEST();

        shared = 0;

        rc = diminuto_thread_start(&odd, (void *)1);
        ASSERT(rc == 0);

        rc = diminuto_thread_start(&even, (void *)0);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = diminuto_thread_join(&odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        final = (void *)~0;
        rc = diminuto_thread_join(&even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        ASSERT(shared == LIMIT);

        STATUS();
    }

    {
        int rc;
        diminuto_thread_t odd;
        diminuto_thread_t even;
        void * final;

        TEST();

        shared = 0;

        ASSERT(diminuto_thread_init(&odd, body3) == &odd);
        ASSERT(diminuto_thread_init(&even, body3) == &even);

        rc = diminuto_thread_start(&odd, (void *)1);
        ASSERT(rc == 0);

        rc = diminuto_thread_start(&even, (void *)0);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = diminuto_thread_join(&odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        final = (void *)~0;
        rc = diminuto_thread_join(&even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        ASSERT(shared == LIMIT);

        ASSERT(diminuto_thread_fini(&even) == (diminuto_thread_t *)0);
        ASSERT(diminuto_thread_fini(&odd) == (diminuto_thread_t *)0);

        STATUS();
    }

    {
        int rc;
        diminuto_thread_t odd = DIMINUTO_THREAD_INITIALIZER(body4);
        diminuto_thread_t even = DIMINUTO_THREAD_INITIALIZER(body4);
        void * final;

        TEST();

        shared = 0;

        rc = diminuto_thread_start(&odd, (void *)1);
        ASSERT(rc == 0);

        rc = diminuto_thread_start(&even, (void *)0);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = diminuto_thread_join(&odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        final = (void *)~0;
        rc = diminuto_thread_join(&even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        ASSERT(shared == LIMIT);

        STATUS();
    }

    {
        int rc;
        diminuto_thread_t thread = DIMINUTO_THREAD_INITIALIZER(body5);
        void * final;
        diminuto_ticks_t ticks;

        TEST();

        rc = diminuto_thread_start(&thread, (void *)5);
        ASSERT(rc == 0);

        COMMENT("STARTED");

        DIMINUTO_THREAD_BEGIN(&thread);
            while (diminuto_thread_state(&thread) != DIMINUTO_THREAD_STATE_RUNNING) {
                COMMENT("WAITING");
                diminuto_thread_wait(&thread);
            }
        DIMINUTO_THREAD_END;

        COMMENT("RUNNING");

        final = (void *)0xdeadbeef;
        ticks = diminuto_thread_clock() + (diminuto_frequency() * 5);
        COMMENT("PAUSING 5s");
        rc = diminuto_thread_join_until(&thread, &final, ticks);
        ASSERT(rc == DIMINUTO_THREAD_TIMEDOUT);
        ASSERT(final == (void *)0xdeadbeef);

        COMMENT("NOTIFYING");
        rc = diminuto_thread_notify(&thread);
        ASSERT(rc == 0);

        final = (void *)~0;
        COMMENT("JOINING");
        rc = diminuto_thread_join(&thread, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)5);

        COMMENT("FINISHED");

        STATUS();
    }

    {
        int rc;
        diminuto_thread_t thread;
        diminuto_thread_t * tp;
        void * final;
        uintptr_t ii;

        TEST();

        tp = diminuto_thread_init(&thread, body5);
        ASSERT(tp == &thread);

        for (ii = 0; ii < 5; ++ii) {

            COMMENT("STARTING %u\n", (unsigned int)ii);

            rc = diminuto_thread_start(&thread, (void *)ii);
            ASSERT(rc == 0);

            COMMENT("STARTED");

            DIMINUTO_THREAD_BEGIN(&thread);
                while (diminuto_thread_state(&thread) != DIMINUTO_THREAD_STATE_RUNNING) {
                    COMMENT("WAITING");
                    diminuto_thread_wait(&thread);
                }
            DIMINUTO_THREAD_END;

            COMMENT("RUNNING");

            COMMENT("NOTIFYING");
            rc = diminuto_thread_notify(&thread);
            ASSERT(rc == 0);

            final = (void *)~0;
            COMMENT("JOINING");
            rc = diminuto_thread_join(&thread, &final);
            ASSERT(rc == 0);
            ASSERT(final == (void *)ii);

        }

        tp = diminuto_thread_fini(&thread);
        ASSERT(tp == (diminuto_thread_t *)0);

        COMMENT("FINISHED");

        STATUS();
    }

    {
        int rc;
        diminuto_thread_t thread;
        void * final;
        diminuto_ticks_t ticks;
        uintptr_t count;
        uintptr_t index;
        static const uintptr_t LIMIT = 8;

        TEST();

        diminuto_thread_init(&thread, body6);

        rc = diminuto_thread_start(&thread, (void *)LIMIT);
        ASSERT(rc == 0);

        COMMENT("STARTED");

        DIMINUTO_THREAD_BEGIN(&thread);
            while (diminuto_thread_state(&thread) != DIMINUTO_THREAD_STATE_RUNNING) {
                COMMENT("WAITING");
                diminuto_thread_wait(&thread);
            }
        DIMINUTO_THREAD_END;

        COMMENT("RUNNING");

        for (count = 0; count < LIMIT; ++count) {
            COMMENT("NOTIFYING");
            rc = diminuto_thread_notify(&thread);
            ASSERT(rc == 0);
            for (index = 0; index < count; ++index) {
                COMMENT("YIELDING");
                diminuto_thread_yield();
            }
        }

        final = (void *)~0;
        ticks = diminuto_thread_clock() + (diminuto_frequency() * 10);
        COMMENT("JOINING");
        rc = diminuto_thread_join_until(&thread, &final, ticks);
        ASSERT(rc == 0);
        ASSERT(final == (void *)6);

        COMMENT("FINISHED");

        STATUS();
    }

    {
        int rc;
        diminuto_thread_t thread;
        void * final;
        diminuto_ticks_t ticks;

        TEST();

        diminuto_thread_init(&thread, body7);

        rc = diminuto_thread_start(&thread, (void *)0);
        ASSERT(rc == 0);

        COMMENT("STARTED");

        ticks = diminuto_thread_clock() + diminuto_frequency();
        DIMINUTO_THREAD_BEGIN(&thread);
            while (diminuto_thread_state(&thread) != DIMINUTO_THREAD_STATE_RUNNING) {
                COMMENT("WAITING");
                rc = diminuto_thread_wait_until(&thread, ticks);
                ASSERT(rc == 0);
            }
        DIMINUTO_THREAD_END;

        COMMENT("RUNNING");

        final = (void *)0xdeadbeef;
        ticks = diminuto_thread_clock() + (diminuto_frequency() * 5);
        COMMENT("PAUSING 5s");
        rc = diminuto_thread_join_until(&thread, &final, ticks);
        ASSERT(rc == DIMINUTO_THREAD_TIMEDOUT);
        ASSERT(final == (void *)0xdeadbeef);

        COMMENT("NOTIFYING");
        rc = diminuto_thread_notify(&thread);
        ASSERT(rc == 0);

        final = (void *)~0;
        ticks = diminuto_thread_clock() + (diminuto_frequency() * 10);
        COMMENT("JOINING");
        rc = diminuto_thread_join_until(&thread, &final, ticks);
        ASSERT(rc == 0);
        ASSERT(final == (void *)7);

        COMMENT("FINISHED");

        STATUS();
    }

    {
        int rc;
        diminuto_thread_t odd;
        diminuto_thread_t even;
        void * final;

        TEST();

        shared = 0;

        ASSERT(diminuto_thread_init(&odd, body8) == &odd);
        ASSERT(diminuto_thread_init(&even, body8) == &even);

        rc = diminuto_thread_start(&odd, (void *)1);
        ASSERT(rc == 0);

        rc = diminuto_thread_start(&even, (void *)0);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = diminuto_thread_join(&odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        final = (void *)~0;
        rc = diminuto_thread_join(&even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        ASSERT(shared == LIMIT);

        ASSERT(diminuto_thread_fini(&even) == (diminuto_thread_t *)0);
        ASSERT(diminuto_thread_fini(&odd) == (diminuto_thread_t *)0);

        STATUS();
    }

    {
        int rc;
        diminuto_thread_t odd;
        diminuto_thread_t even;
        void * final;

        TEST();

        shared = 0;

        ASSERT(diminuto_thread_init(&even, body8) == &even);
        ASSERT(diminuto_thread_init(&odd, body8) == &odd);

        rc = diminuto_thread_start(&even, (void *)0);
        ASSERT(rc == 0);

        rc = diminuto_thread_start(&odd, (void *)1);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = diminuto_thread_join(&even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        final = (void *)~0;
        rc = diminuto_thread_join(&odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        ASSERT(shared == LIMIT);

        ASSERT(diminuto_thread_fini(&odd) == (diminuto_thread_t *)0);
        ASSERT(diminuto_thread_fini(&even) == (diminuto_thread_t *)0);

        STATUS();
    }

    {
        int rc;
        diminuto_thread_t odd;
        diminuto_thread_t even;
        void * final;

        TEST();

        shared = 0;

        ASSERT(diminuto_thread_init_generic(&even, body8) == &even);
        ASSERT(diminuto_thread_init_generic(&odd, body8) == &odd);

        rc = diminuto_thread_start(&even, (void *)0);
        ASSERT(rc == 0);

        rc = diminuto_thread_start(&odd, (void *)1);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = diminuto_thread_join(&even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        final = (void *)~0;
        rc = diminuto_thread_join(&odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        ASSERT(shared == LIMIT);

        ASSERT(diminuto_thread_fini(&odd) == (diminuto_thread_t *)0);
        ASSERT(diminuto_thread_fini(&even) == (diminuto_thread_t *)0);

        STATUS();
    }

    /*
     * Only root processes can set an explicit scheduler for threads.
     * So we only run this test if this unit test is being run as root,
     * or there is a command line flag to override this.
     */

    if ((geteuid() == 0) || ((argc > 1) && (strcmp(argv[1], "root") == 0)))
    {
        int rc;
        diminuto_thread_t odd;
        diminuto_thread_t even;
        void * final;

        TEST();

        shared = 0;

        ASSERT(diminuto_thread_init_base(&even, body8, DIMINUTO_POLICY_SCHEDULER_THREAD, DIMINUTO_POLICY_PRIORITY_THREAD) == &even);
        ASSERT(diminuto_thread_init_base(&odd, body8, DIMINUTO_POLICY_SCHEDULER_THREAD, DIMINUTO_POLICY_PRIORITY_THREAD) == &odd);

        rc = diminuto_thread_start(&even, (void *)0);
        ASSERT(rc == 0);

        rc = diminuto_thread_start(&odd, (void *)1);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = diminuto_thread_join(&even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        final = (void *)~0;
        rc = diminuto_thread_join(&odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        ASSERT(shared == LIMIT);

        ASSERT(diminuto_thread_fini(&odd) == (diminuto_thread_t *)0);
        ASSERT(diminuto_thread_fini(&even) == (diminuto_thread_t *)0);

        STATUS();
    }

    EXIT();
}
