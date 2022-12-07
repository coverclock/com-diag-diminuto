/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Mutex feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Mutex feature.
 *
 * Note that Diminuto Mutexen are recursive.
 */

#include "com/diag/diminuto/diminuto_mutex.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <pthread.h>
#include <stdint.h>
#include <errno.h>

static int shared = 0;
static const int LIMIT = 100;

static void * body1(void * arg)
{
    static diminuto_mutex_t mutex = DIMINUTO_MUTEX_INITIALIZER;
    int done = 0;

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
    int successful = 0;
    int busy = 0;

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
    diminuto_log_setmask();

    {
        TEST();

        ASSERT(DIMINUTO_MUTEX_ERROR == EIO);
        ASSERT(DIMINUTO_MUTEX_BUSY == EBUSY);

        STATUS();
    }

    {
        diminuto_mutex_t mutex1;
        diminuto_mutex_t mutex2;

        TEST();

        ASSERT(diminuto_mutex_init(&mutex1) == &mutex1);
        ASSERT(diminuto_mutex_init(&mutex2) == &mutex2);

        ASSERT(diminuto_mutex_lock_try(&mutex1) == 0);
        ASSERT(diminuto_mutex_unlock(&mutex1) == 0);
        ASSERT(diminuto_mutex_lock_try(&mutex2) == 0);
        ASSERT(diminuto_mutex_unlock(&mutex2) == 0);

        ASSERT(diminuto_mutex_lock_try(&mutex1) == 0);
        ASSERT(diminuto_mutex_lock_try(&mutex2) == 0);
        ASSERT(diminuto_mutex_unlock(&mutex1) == 0);
        ASSERT(diminuto_mutex_unlock(&mutex2) == 0);

        ASSERT(diminuto_mutex_lock(&mutex1) == 0);
            ASSERT(diminuto_mutex_lock(&mutex2) == 0);
                ASSERT(diminuto_mutex_lock(&mutex1) == 0);
                    ASSERT(diminuto_mutex_lock(&mutex2) == 0);
                    ASSERT(diminuto_mutex_unlock(&mutex2) == 0);;
                ASSERT(diminuto_mutex_unlock(&mutex1) == 0);
            ASSERT(diminuto_mutex_unlock(&mutex2) == 0);;
        ASSERT(diminuto_mutex_unlock(&mutex1) == 0);

        ASSERT(diminuto_mutex_fini(&mutex2) == (diminuto_mutex_t *)0);
        ASSERT(diminuto_mutex_fini(&mutex1) == (diminuto_mutex_t *)0);

        STATUS();
    }

    {
        int rc;
        pthread_t odd;
        pthread_t even;
        void * final;

        TEST();

        shared = 0;

        rc = pthread_create(&odd, 0, body1, (void *)1);
        ASSERT(rc == 0);

        rc = pthread_create(&even, 0, body1, (void *)0);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = pthread_join(odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        final = (void *)~0;
        rc = pthread_join(even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        ASSERT(shared == LIMIT);

        STATUS();
    }

    {
        int rc;
        pthread_t odd;
        pthread_t even;
        void * final;

        TEST();

        shared = 0;

        rc = pthread_create(&odd, 0, body2, (void *)1);
        ASSERT(rc == 0);

        rc = pthread_create(&even, 0, body2, (void *)0);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = pthread_join(odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        final = (void *)~0;
        rc = pthread_join(even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        ASSERT(shared == LIMIT);

        STATUS();
    }

    {
        int rc;
        pthread_t odd;
        pthread_t even;
        void * final;

        TEST();

        shared = 0;

        rc = pthread_create(&odd, 0, body3, (void *)1);
        ASSERT(rc == 0);

        rc = pthread_create(&even, 0, body3, (void *)0);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = pthread_join(odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        final = (void *)~0;
        rc = pthread_join(even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        ASSERT(shared == LIMIT);

        STATUS();
    }

    {
        int rc;
        pthread_t odd;
        pthread_t even;
        void * final;

        TEST();

        shared = 0;

        rc = pthread_create(&odd, 0, body4, (void *)1);
        ASSERT(rc == 0);

        rc = pthread_create(&even, 0, body4, (void *)0);
        ASSERT(rc == 0);

        final = (void *)~0;
        rc = pthread_join(odd, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)1);

        final = (void *)~0;
        rc = pthread_join(even, &final);
        ASSERT(rc == 0);
        ASSERT(final == (void *)0);

        ASSERT(shared == LIMIT);

        STATUS();
    }

    {
        diminuto_mutex_t mutex;
        int rc;

        TEST();

        ASSERT(diminuto_mutex_init(&mutex) == &mutex);

        rc = 0;
        ASSERT(diminuto_mutex_lock_try(&mutex) == 0);
            rc = 1;
        ASSERT(diminuto_mutex_unlock(&mutex) == 0);
        ASSERT(rc == 1);

        rc = 0;
        ASSERT(diminuto_mutex_lock_try(&mutex) == 0);
            rc = 1;
            if (diminuto_mutex_lock_try(&mutex) == 0) {
                rc = 2;
            	ASSERT(diminuto_mutex_unlock(&mutex) == 0);
            }
        ASSERT(diminuto_mutex_unlock(&mutex) == 0);
        ASSERT(rc == 2);

        rc = 0;
        ASSERT(diminuto_mutex_lock_try(&mutex) == 0);
            rc = 1;
            if (diminuto_mutex_lock_try(&mutex) == 0) {
                rc = 2;
            	ASSERT(diminuto_mutex_unlock(&mutex) == 0);
            }
        ASSERT(diminuto_mutex_unlock(&mutex) == 0);
        ASSERT(rc == 2);

        ASSERT(diminuto_mutex_fini(&mutex) == (diminuto_mutex_t *)0);

        STATUS();
    }

    EXIT();
}
