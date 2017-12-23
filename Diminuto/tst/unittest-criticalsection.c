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

int main(void)
{
	pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

	ASSERT(pthread_mutex_trylock(&mutex1) == 0);
	pthread_mutex_unlock(&mutex1);
	ASSERT(pthread_mutex_trylock(&mutex2) == 0);
	pthread_mutex_unlock(&mutex2);

	DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex1);

		ASSERT(pthread_mutex_trylock(&mutex1) == EBUSY);
		ASSERT(pthread_mutex_trylock(&mutex2) == 0);
		pthread_mutex_unlock(&mutex2);

		DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex2);

			ASSERT(pthread_mutex_trylock(&mutex1) == EBUSY);
			ASSERT(pthread_mutex_trylock(&mutex2) == EBUSY);

		DIMINUTO_CRITICAL_SECTION_END;

		ASSERT(pthread_mutex_trylock(&mutex1) == EBUSY);
		ASSERT(pthread_mutex_trylock(&mutex2) == 0);
		pthread_mutex_unlock(&mutex2);

	DIMINUTO_CRITICAL_SECTION_END;

	ASSERT(pthread_mutex_trylock(&mutex1) == 0);
	pthread_mutex_unlock(&mutex1);
	ASSERT(pthread_mutex_trylock(&mutex2) == 0);
	pthread_mutex_unlock(&mutex2);

    EXIT();
}
