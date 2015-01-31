/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * zulu [ -n ]<BR>
 *
 * EXAMPLES
 *
 * zulu<BR>
 * zulu -n<BR>
 *
 * ABSTRACT
 *
 * Prints on standard output the UTC time in either an ISO8601 timestamp or
 * as a number of ticks.
 */

#include <stdio.h>
#include <string.h>
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"

int main(int argc, char ** argv)
{
	int numeric = 0;
	diminuto_ticks_t ticks;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int fraction;

	if (argc <= 1) {
		/* Do nothing. */
	} else if (strcmp(argv[1], "-n") == 0) {
		numeric = !0;
	} else {
		fprintf(stderr, "usage: %s [ -n ]\n", argv[0]);
		return 1;
	}

	if ((ticks = diminuto_time_clock()) < 0) {
		return 2;
	}

	if (numeric) {
		printf("%lld\n", ticks);
	} else if (diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &fraction) < 0) {
		return 2;
	} else {
		printf("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9dZ\n", year, month, day, hour, minute, second, fraction);
	}

	return 0;
}
