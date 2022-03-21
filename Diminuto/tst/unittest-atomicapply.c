/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the (minimal) unit test for the Atomic Apply.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * EXPERIMENTAL
 */

#include "com/diag/diminuto/diminuto_atomicapply.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdint.h>

/*
 * LIBRARY: PRIVATE
 */

static pthread_mutex_t library_mutex = PTHREAD_MUTEX_INITIALIZER;

static intptr_t library_database = 0;

static intptr_t library_settor(diminuto_atomic_functor_t * fp, intptr_t value) {
    extern pthread_mutex_t library_mutex;
    extern intptr_t library_database;
    return (intptr_t)diminuto_atomic_apply(&library_mutex, fp, &library_database, (void *)value);
}

/*
 * LIBRARY: PUBLIC
 */

extern intptr_t library_settor(diminuto_atomic_functor_t * fp, intptr_t value);

/*
 * APPLICATION
 */

static void * application_functor(void * dp, void * cp) {
	void * rp = (void *)*(intptr_t *)dp;
    *(intptr_t *)dp += (intptr_t)cp;
    return rp;
}

void main(void)
{
    SETLOGMASK();

    {
        TEST();

        ASSERT(library_settor(application_functor,  1) == 0);
        ASSERT(library_settor(application_functor,  2) == 1);
        ASSERT(library_settor(application_functor, -3) == 3);

        STATUS();
    }

    EXIT();
}
