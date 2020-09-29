/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include <pthread.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

static int shared = 0;
static const int LIMIT = 100;

static void * body1(void * arg)
{
    static diminuto_mutex_t mutex = DIMINUTO_MUTEX_INITIALIZER;
    int done = 0;

    ASSERT(diminuto_thread_instance() != (diminuto_thread_t *)0);

    while (!done) {

        ASSERT(diminuto_mutex_lock(&mutex) == 0);

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d\n", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d\n", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
            } else {
                /* Do nothing. */
            }

        ASSERT(diminuto_mutex_unlock(&mutex) == 0);

    }

    return (void *)arg;
}

static void * body2(void * arg)
{
    static diminuto_mutex_t mutex = DIMINUTO_MUTEX_INITIALIZER;
    int done = 0;
    int rc = -1;
    int busy = 0;

    ASSERT(diminuto_thread_instance() != (diminuto_thread_t *)0);

    while (!done) {

        rc = diminuto_mutex_lock_try(&mutex);
        ASSERT((rc == 0) || (rc == DIMINUTO_MUTEX_BUSY));
        if (rc == 0) {

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d\n", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d\n", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
            } else {
                /* Do nothing. */
            }

            ASSERT(diminuto_mutex_unlock(&mutex) == 0);
        } else {
            COMMENT("BUSY\n");
            busy += 1;
        }

    }

    COMMENT("busy: %d\n", busy);
    ASSERT(busy > 0);

    return (void *)arg;
}

static void * body3(void * arg)
{
    static diminuto_mutex_t mutex = DIMINUTO_MUTEX_INITIALIZER;
    int done = 0;

    ASSERT(diminuto_thread_instance() != (diminuto_thread_t *)0);

    while (!done) {

        DIMINUTO_MUTEX_BEGIN(&mutex);

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d\n", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d\n", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
            } else {
                /* Do nothing. */
            }

        DIMINUTO_MUTEX_END;

    }

    return (void *)arg;
}

static void * body4(void * arg)
{
    static diminuto_mutex_t mutex = DIMINUTO_MUTEX_INITIALIZER;
    int done = 0;
    int rc = -1;
    int successful = 0;
    int busy = 0;

    ASSERT(diminuto_thread_instance() != (diminuto_thread_t *)0);

    while (!done) {

        successful = 0;

        DIMINUTO_MUTEX_TRY(&mutex);

            successful = !0;

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d\n", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d\n", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
            } else {
                /* Do nothing. */
            }

        DIMINUTO_MUTEX_END;

        if (!successful) {
            COMMENT("BUSY\n");
            busy += 1;
        }

    }

    COMMENT("busy: %d\n", busy);
    ASSERT(busy > 0);

    return (void *)arg;
}

int main(void)
{
    {
        ASSERT(pthread_equal(pthread_self(), diminuto_thread_self()));
        ASSERT(diminuto_thread_yield() == 0);
        ASSERT(diminuto_thread_notified() == 0);
    }

    {
        diminuto_thread_t * tp;
        ASSERT((tp = diminuto_thread_instance()) != (diminuto_thread_t *)0);
        ASSERT(pthread_equal(tp->thread, diminuto_thread_self()));
        ASSERT(tp->function == (void * (*)(void *))main);
        ASSERT(tp->context == (void *)0);
        ASSERT(tp->value == (void *)~0);
        ASSERT(tp->state == DIMINUTO_THREAD_STATE_RUNNING);
        ASSERT(tp->notification == 0);
        ASSERT(tp->notifying == 0);
    }

    {
        diminuto_thread_t thread = DIMINUTO_THREAD_INITIALIZER(body1);
        ASSERT(thread.function == body1);
        ASSERT(thread.context == (void *)0);
        ASSERT(thread.value == (void *)~0);
        ASSERT(thread.state == DIMINUTO_THREAD_STATE_INITIALIZED);
        ASSERT(thread.notification == DIMINUTO_THREAD_SIGNAL);
        ASSERT(thread.notifying == 0);
    }

    {
        diminuto_thread_t thread = { 0, };
        ASSERT(thread.state == DIMINUTO_THREAD_STATE_ALLOCATED);
        ASSERT(diminuto_thread_init(&thread, body2) == &thread);
        ASSERT(thread.function == body2);
        ASSERT(thread.context == (void *)0);
        ASSERT(thread.value == (void *)~0);
        ASSERT(thread.state == DIMINUTO_THREAD_STATE_INITIALIZED);
        ASSERT(thread.notification == DIMINUTO_THREAD_SIGNAL);
        ASSERT(thread.notifying == 0);
        ASSERT(diminuto_thread_fini(&thread) == (diminuto_thread_t *)0);
        ASSERT(thread.state == DIMINUTO_THREAD_STATE_FINALIZED);
    }

    {
        int rc;
        diminuto_thread_t odd;
        diminuto_thread_t even;
        void * final;

        TEST();

        shared = 0;

        ASSERT(diminuto_thread_init(&odd, body1) == &odd);
        ASSERT(diminuto_thread_init(&even, body1) == &even);

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
        diminuto_thread_t odd = DIMINUTO_THREAD_INITIALIZER(body2);
        diminuto_thread_t even = DIMINUTO_THREAD_INITIALIZER(body2);
        void * final;

        TEST();

        shared = 0;

        rc = diminuto_thread_start(&odd, (void *)1);
        COMMENT("%p='%c' rc=%d=\"%s\" errno=%d=\"%s\"\n", &odd, odd.state, rc, strerror(rc), errno, strerror(errno));
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

    EXIT();
}
