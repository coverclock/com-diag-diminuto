/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_time.h"
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

static void epoch(diminuto_usec_t now)
{
	int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int microsecond;
    diminuto_usec_t zulu;
    diminuto_usec_t juliet;
    diminuto_usec_t timezone;
    diminuto_usec_t daylightsaving;
    diminuto_usec_t hertz;
    int zh;
    int zm;
    int dh;
    int dm;
    static int prior = -1;

    diminuto_time_zulu(now, &year, &month, &day, &hour, &minute, &second, &microsecond);
    zulu = diminuto_time_epoch(year, month, day, hour, minute, second, microsecond, 0, 0);
    diminuto_time_juliet(now, &year, &month, &day, &hour, &minute, &second, &microsecond);
    timezone = diminuto_time_timezone(now);
    daylightsaving = diminuto_time_daylightsaving(now);
    juliet = diminuto_time_epoch(year, month, day, hour, minute, second, microsecond, timezone, daylightsaving);
    hertz = diminuto_time_hertz();
    zh = (-timezone / hertz) / 3600;
    zm = (-timezone / hertz) % 3600;
    dh = (daylightsaving / hertz) / 3600;
    dm = (daylightsaving / hertz) % 3600;
    if (year != prior) {
    	printf("%4.4d\n", year);
    	prior = year;
    }
	if ((now != zulu) || (now != juliet)) {
    	printf("%10lld %10lld %10lld %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%6.6d-%2.2d:%2.2d+%2.2d:%2.2d\n", now, zulu, juliet, year, month, day, hour, minute, second, microsecond, zh, zm, dh, dm);
    	exit(1);
    }
}

static const diminuto_usec_t LOW = 0xffffffff80000000LL;
static const diminuto_usec_t HIGH = 0x000000007fffffffLL - (7 * 3600) - 3600;

int main(int argc, char ** argv)
{
    diminuto_usec_t now;
    diminuto_usec_t hertz;

    diminuto_core_enable();

    hertz = diminuto_time_hertz();

    for (now = LOW; now <= HIGH; now += (365 * 24 * 60 * 60)) {
    	epoch(now * hertz);
     }

    for (now = LOW; now <= HIGH; now += (24 * 60 * 60)) {
    	epoch(now * hertz);
    }

    for (now = LOW; now <= HIGH; now += (60 * 60)) {
    	epoch(now * hertz);
    }

    for (now = LOW; now <= HIGH; now += 60) {
    	epoch(now * hertz);
    }

    for (now = LOW; now <= HIGH; now += 1) {
    	epoch(now * hertz);
    }

    return 0;
}
