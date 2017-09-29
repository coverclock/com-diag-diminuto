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
 * juliet<BR>
 *
 * EXAMPLES
 *
 * juliet<BR>
 * juliet -n<BR>
 *
 * ABSTRACT
 *
 * Prints on standard output the local time as an ISO8601 timestamp.
 */

#include <stdio.h>
#include <string.h>
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"

int main(int argc, char ** argv)
{
	diminuto_ticks_t ticks;
	diminuto_ticks_t offset;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	diminuto_ticks_t fraction;
	int offsethours;
	int offsetseconds;

	if ((ticks = diminuto_time_clock()) < 0) {
		return 2;
	}

	offset = diminuto_time_timezone(ticks) + diminuto_time_daylightsaving(ticks);
	second = diminuto_frequency_ticks2wholeseconds(offset);
	if (second < 0) { second = -second; }
	offsethours = second / 3600;
	offsetseconds = second % 3600;

	if (diminuto_time_juliet(ticks, &year, &month, &day, &hour, &minute, &second, &fraction) < 0) {
		return 3;
	}

	printf("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9llu%c%2.2d:%2.2d\n", year, month, day, hour, minute, second, fraction, (offset < 0) ? '-' : '+', offsethours, offsetseconds);

	return 0;
}
