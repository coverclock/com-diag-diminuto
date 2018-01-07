/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include <pthread.h>
#include <stdint.h>


static void * body(void * arg)
{
	static int shared = 0;

	do {

		int temporary;

		DIMINUTO_COHERENT_SECTION_BEGIN;

			temporary = shared;

			if ((shared % 2) == (intptr_t)arg) {
				diminuto_log_emit("%s sees %d\n", (intptr_t)arg ? "odd " : "even", shared);
				++shared;
			}

		DIMINUTO_COHERENT_SECTION_END;

		if (temporary >= 100) {
			break;
		}

		diminuto_delay(diminuto_frequency() / 10, !0);

	} while (!0);

	return (void *)0;
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

		rc = pthread_create(&odd, 0, body, (void *)1);
		ASSERT(rc == 0);

		rc = pthread_create(&even, 0, body, (void *)0);
		ASSERT(rc == 0);

		rc = pthread_join(odd, &final);
		ASSERT(rc == 0);
		ASSERT(final == (void *)0);

		rc = pthread_join(even, &final);
		ASSERT(rc == 0);
		ASSERT(final == (void *)0);

		STATUS();
	}

    EXIT();
}
