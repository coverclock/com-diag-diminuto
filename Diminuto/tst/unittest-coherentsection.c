/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include <pthread.h>
#include <stdint.h>

/*
 * This really isn't an adequate test. At best it just tests that the feature
 * doesn't break a simple algorithm.
 */

static int shared = 0;
static const int LIMIT = 100;

static void * body(void * arg)
{
    int done = 0;

    while (!done) {

        DIMINUTO_COHERENT_SECTION_BEGIN;

            if (shared >= LIMIT) {
                COMMENT("%s saw  %d\n", (intptr_t)arg ? "odd " : "even", shared);
                done = !0;
            } else if ((shared % 2) == (intptr_t)arg) {
                COMMENT("%s sees %d\n", (intptr_t)arg ? "odd " : "even", shared);
                ++shared;
            } else {
                diminuto_yield();
            }

        DIMINUTO_COHERENT_SECTION_END;

    }

    return (void *)arg;
}

int main(void)
{

    {
        TEST();

        DIMINUTO_COHERENT_SECTION_BEGIN;

            int temp = 0;

        DIMINUTO_COHERENT_SECTION_END;

        DIMINUTO_COHERENT_SECTION_BEGIN;

            int temp = 1;

            DIMINUTO_COHERENT_SECTION_BEGIN;

                int temp = 2;

            DIMINUTO_COHERENT_SECTION_END;

            ASSERT(temp == 1);

        DIMINUTO_COHERENT_SECTION_END;

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

    EXIT();
}
