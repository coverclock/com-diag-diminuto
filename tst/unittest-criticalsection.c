/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
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
	int state;

	ASSERT(pthread_mutex_trylock(&mutex) == 0);
	pthread_mutex_unlock(&mutex);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &state);
	ASSERT(state == PTHREAD_CANCEL_ENABLE);

	DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

		ASSERT(pthread_mutex_trylock(&mutex) == EBUSY);
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &state);
		ASSERT(state == PTHREAD_CANCEL_DISABLE);

	DIMINUTO_CRITICAL_SECTION_END;

	ASSERT(pthread_mutex_trylock(&mutex) == 0);
	pthread_mutex_unlock(&mutex);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &state);
	ASSERT(state == PTHREAD_CANCEL_ENABLE);

    return 0;
}
