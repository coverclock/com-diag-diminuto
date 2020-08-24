/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include <pthread.h>
#include <stdint.h>
#include <errno.h>

/*
 * This really isn't an adequate test. At best it just tests that the feature
 * doesn't break a simple algorithm.
 */

static int shared = 0;
static const int LIMIT = 100;

static void * body(void * arg)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    int done = 0;

    while (!done) {

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d\n", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d\n", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
            } else {
                diminuto_yield();
            }

        DIMINUTO_CRITICAL_SECTION_END;

    }

    return (void *)arg;
}

int main(void)
{

    {
        pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

        TEST();

        ASSERT(pthread_mutex_trylock(&mutex1) == 0);
        pthread_mutex_unlock(&mutex1);
        ASSERT(pthread_mutex_trylock(&mutex2) == 0);
        pthread_mutex_unlock(&mutex2);

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex1);

            int error = EBUSY;

            ASSERT(pthread_mutex_trylock(&mutex1) == error);
            ASSERT(pthread_mutex_trylock(&mutex2) == 0);
            pthread_mutex_unlock(&mutex2);

            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex2);

                int error = EBUSY;

                ASSERT(pthread_mutex_trylock(&mutex1) == error);
                ASSERT(pthread_mutex_trylock(&mutex2) == error);

            DIMINUTO_CRITICAL_SECTION_END;

            ASSERT(pthread_mutex_trylock(&mutex1) == error);
            ASSERT(pthread_mutex_trylock(&mutex2) == 0);
            pthread_mutex_unlock(&mutex2);

        DIMINUTO_CRITICAL_SECTION_END;

        ASSERT(pthread_mutex_trylock(&mutex1) == 0);
        pthread_mutex_unlock(&mutex1);
        ASSERT(pthread_mutex_trylock(&mutex2) == 0);
        pthread_mutex_unlock(&mutex2);

        STATUS();
    }

    {
        int rc;
        pthread_t odd;
        pthread_t even;
        void * final;

        TEST();

        ASSERT(shared == 0);

        rc = pthread_create(&odd, 0, body, (void *)1);
        ASSERT(rc == 0);

        rc = pthread_create(&even, 0, body, (void *)0);
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
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        int rc = -1;

        TEST();

        rc = 0;
        DIMINUTO_CRITICAL_SECTION_TRY(&mutex);
            rc = 1;
        DIMINUTO_CRITICAL_SECTION_END;
        ASSERT(rc == 1);

        rc = 0;
        DIMINUTO_CRITICAL_SECTION_TRY(&mutex);
            rc = 1;
            DIMINUTO_CRITICAL_SECTION_TRY(&mutex);
                rc = 2;
            DIMINUTO_CRITICAL_SECTION_END;
        DIMINUTO_CRITICAL_SECTION_END;
        ASSERT(rc == 1);

        rc = 0;
        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
            rc = 1;
            DIMINUTO_CRITICAL_SECTION_TRY(&mutex);
                rc = 2;
            DIMINUTO_CRITICAL_SECTION_END;
        DIMINUTO_CRITICAL_SECTION_END;
        ASSERT(rc == 1);

        STATUS();
    }

    EXIT();
}
