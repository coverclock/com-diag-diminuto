/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <pthread.h>
#include <errno.h>

/*
 * This is mostly just to insure it compiles.
 */

int main(void)
{
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	ASSERT(pthread_mutex_trylock(&mutex) == 0);
	pthread_mutex_unlock(&mutex);

	DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

		ASSERT(pthread_mutex_trylock(&mutex) == EBUSY);

	DIMINUTO_CRITICAL_SECTION_END;

	ASSERT(pthread_mutex_trylock(&mutex) == 0);
	pthread_mutex_unlock(&mutex);

    EXIT();
}
