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
 * usecsleep microseconddelay [ microsecondtime ]<BR>
 *
 * EXAMPLES
 *
 * usecsleep 1000000<BR>
 * usecsleep 1000000 1296241349391021<BR>
 * USEC=`usectime`; sleep 1; time usecsleep 2000000 $USEC
 *
 * ABSTRACT
 *
 * usecsleep in its simplest application merely sleeps the calling process for
 * at least the number of microseconds specified by the first parameter.
 * Whether the underlying operating system actually supports that resolution of
 * process suspension is another matter.
 *
 * In its more complicated application, usecsleep generates the current time in
 * microseconds since the epoch, subtracts the second parameter from it,
 * subtracts that result from the first parameter, and sleeps the calling
 * process for at least that number of microseconds. The second parameter can
 * be generated in a script using the usectime utility. This allows a script to
 * run more or less periodically by coarsely adjusting for jitter introduced
 * by the prior execution of the script. This turns out to be remarkably
 * useful when script periodicity is small, where small in this context means
 * one second or so.
 *
 * This program guarantees that it will always yield the processor. To do
 * otherwise might result in a tight CPU loop in the event of a failure.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_number.h"

void usage(const char * pgm, FILE * fp)
{
    fprintf(fp, "Usage: %s microseconddelay [ microsecondtime ]\n", pgm);
    fprintf(fp, "       %s 1000000\n", pgm);
    fprintf(fp, "       %s 1000000 1296241349391021\n", pgm);
}

int main(int argc, char ** argv)
{
	int rc = 1;
    diminuto_usec_t then;
    diminuto_usec_t now;
    diminuto_usec_t delay;

    if ((argc == 2) && (strcmp(argv[1], "-?") == 0)) {
        usage(argv[0], stdout);
        return 0;
    }

    do {

		if (!((2 <= argc) && (argc <= 3))) {
			usage(argv[0], stderr);
			break;
		}

		if (*diminuto_number_unsigned(argv[1], &delay) != '\0') {
			errno = EINVAL;
			perror(argv[1]);
			break;
		}

		if (argc == 3) {

			if (*diminuto_number_unsigned(argv[2], &then) != '\0') {
				errno = EINVAL;
				perror(argv[2]);
				break;
			}

			if ((now = diminuto_time()) == 0) {
				break;
			}

			if (then > now) {
				errno = EINVAL;
				perror(argv[2]);
				break;
			}

			delay -= now - then;

		}

		if (delay == 0) {
			rc = 0;
			break;
		}

		if (diminuto_delay(delay, 0) != 0) {
			break;
		}

		return 0;

    } while (0);

    diminuto_yield();

    return rc;
}
