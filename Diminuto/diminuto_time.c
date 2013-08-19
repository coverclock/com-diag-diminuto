/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

diminuto_usec_t diminuto_time_clock()
{
    struct timeval clock;
    diminuto_usec_t microseconds = ~0ULL;

    if (gettimeofday(&clock, (struct timezone *)0) < 0) {
        diminuto_perror("diminuto_time_clock: gettimeofday");
    } else {
        microseconds = clock.tv_sec;
        microseconds *= 1000000ULL;
        microseconds += clock.tv_usec;
    }

    return microseconds;
}

static diminuto_usec_t diminuto_time_generic(clockid_t clock)
{
	struct timespec elapsed;
	diminuto_usec_t microseconds = ~0ULL;

	if (clock_gettime(clock, &elapsed) < 0) {
		diminuto_perror("diminuto_time_generic: clock_gettime");
	} else {
		microseconds = elapsed.tv_sec;
		microseconds *= 1000000ULL;
		microseconds += (elapsed.tv_nsec / 1000);
	}

	return microseconds;
}

diminuto_usec_t diminuto_time_elapsed()
{
#if defined(CLOCK_MONOTONIC_RAW)
	return diminuto_time_generic(CLOCK_MONOTONIC_RAW); /* Since Linux 2.6.28 */
#else
	return diminuto_time_generic(CLOCK_MONOTONIC); /* Prior to Linux 2.6.28 */
#endif
}

diminuto_usec_t diminuto_time_process()
{
	return diminuto_time_generic(CLOCK_PROCESS_CPUTIME_ID);
}

diminuto_usec_t diminuto_time_thread()
{
	return diminuto_time_generic(CLOCK_THREAD_CPUTIME_ID);
}
