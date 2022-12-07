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
 * LIBRARY: PRIVATE IMPLEMENTATION
 */

static pthread_mutex_t library_mutex = PTHREAD_MUTEX_INITIALIZER;

static int library_database = 0;

/*
 * LIBRARY: PUBLIC API
 */

static inline int library_settor(diminuto_atomic_functor_t * fp, int value) {
    extern pthread_mutex_t library_mutex;
    extern int library_database;
    void * result;
    result = diminuto_atomic_apply(&library_mutex, fp, &library_database, (void *)(intptr_t)value);
    ASSERT(result != DIMINUTO_ATOMIC_ERROR);
    return (int)(intptr_t)result;
}

/*
 * APPLICATION: FUNCTOR
 */

static void * application_functor(void * dp, void * cp) {
    int * ip = (int *)dp;
	void * rp = (void *)(intptr_t)*ip;
    int vp = (intptr_t)cp;
    *ip += vp;
    return rp;
}

int main(void)
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
