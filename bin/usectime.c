/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2011-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * usectime<BR>
 *
 * EXAMPLES
 *
 * usectime<BR>
 *
 * ABSTRACT
 *
 * Prints on standard output the number of microseconds since the Epoch.
 */

#include <stdio.h>
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"

int main(int argc, char ** argv)
{
	diminuto_ticks_t ticks;

	if ((ticks = diminuto_time_clock()) == 0) {
		return 1;
	}

	printf("%llu\n", COM_DIAG_DIMINUTO_TICKS_FROM(ticks, 1000000));

    return 0;
}
