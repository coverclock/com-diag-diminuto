/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2011-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * USAGE
 *
 * elapsedtime [ UNITS ]<BR>
 *
 * EXAMPLES
 *
 * elapsedtime<BR>
 * elapsedtime 1<BR>
 * elapsedtime 1000<BR>
 * elapsedtime 1000000<BR>
 * elapsedtime 1000000000<BR>
 *
 * ABSTRACT
 *
 * Prints on standard output the elapsed time of the rate
 * monotonic clock in the specified units. Defaults to seconds.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"

int main(int argc, char ** argv)
{
	diminuto_sticks_t ticks;
	diminuto_sticks_t units;
	char * endptr = 0;

	if (argc < 2) {
		units = 1;
	} else if ((units = strtoll(argv[1], &endptr, 0)) <= 0) {
		errno = EINVAL;
		diminuto_perror(argv[1]);
		return 1;
	} else if ((endptr == (char *)0) || (*endptr != '\0')) {
		errno = EINVAL;
		diminuto_perror(argv[1]);
		return 1;
	} else {
		/* Do nothing. */
	}

	if ((ticks = diminuto_time_elapsed()) == 0) {
		return 2;
	}

	printf("%lld\n", diminuto_frequency_ticks2units(ticks, units));

    return 0;
}

