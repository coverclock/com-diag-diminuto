/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2011 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
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
#include "diminuto/diminuto_time.h"

int main(int argc, char ** argv)
{
	diminuto_usec_t ticks;

	if ((ticks = diminuto_time()) == 0) {
		return 1;
	}

	printf("%llu\n", ticks);

    return 0;
}
