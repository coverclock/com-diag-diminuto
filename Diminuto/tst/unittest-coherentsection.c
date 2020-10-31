/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Coherent Section feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Coherent Section feature.
 */

#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_thread.h"
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
        diminuto_thread_t odd;
        diminuto_thread_t even;
        diminuto_thread_t * tp;
        void * final;

        TEST();

        ASSERT(shared == 0);

        tp = diminuto_thread_init(&odd, body);
        ASSERT(tp == &odd);
        tp = diminuto_thread_init(&even, body);
        ASSERT(tp == &even);

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

        tp = diminuto_thread_fini(&odd);
        ASSERT(tp == (diminuto_thread_t *)0);
        tp = diminuto_thread_fini(&even);
        ASSERT(tp == (diminuto_thread_t *)0);

        STATUS();
    }

    EXIT();
}
