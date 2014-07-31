/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_time.h"
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

static void epoch(diminuto_ticks_t now, int verbose)
{
	int dday = -1;
	int dhour = -1;
	int dminute = -1;
	int dsecond = -1;
	int dtick = -1;
	int zyear = -1;
    int zmonth = -1;
    int zday = -1;
    int zhour = -1;
    int zminute = -1;
    int zsecond = -1;
    int ztick = -1;
	int jyear = -1;
    int jmonth = -1;
    int jday = -1;
    int jhour = -1;
    int jminute = -1;
    int jsecond = -1;
    int jtick = -1;
    diminuto_ticks_t zulu;
    diminuto_ticks_t juliet;
    diminuto_ticks_t timezone;
    diminuto_ticks_t daylightsaving;
    diminuto_ticks_t hertz;
    int zh;
    int zm;
    int dh;
    int dm;
    int rc;
    static int prior = -1;

    diminuto_time_zulu(now, &zyear, &zmonth, &zday, &zhour, &zminute, &zsecond, &ztick);
    zulu = diminuto_time_epoch(zyear, zmonth, zday, zhour, zminute, zsecond, ztick, 0, 0);
    diminuto_time_juliet(now, &jyear, &jmonth, &jday, &jhour, &jminute, &jsecond, &jtick);
    timezone = diminuto_time_timezone(now);
    daylightsaving = diminuto_time_daylightsaving(now);
    juliet = diminuto_time_epoch(jyear, jmonth, jday, jhour, jminute, jsecond, jtick, timezone, daylightsaving);
    hertz = diminuto_time_frequency();
    zh = (-timezone / hertz) / 3600;
    zm = (-timezone / hertz) % 3600;
    dh = (daylightsaving / hertz) / 3600;
    dm = (daylightsaving / hertz) % 3600;
    rc = diminuto_time_duration(now, &dday, &dhour, &dminute, &dsecond, &dtick);
    if (rc < 0) { dday = -dday; }
	if ((now != zulu) || (now != juliet) || verbose || (zyear != prior)) {
    	DIMINUTO_LOG_DEBUG("%20lld %20lld %20lld %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9d-%2.2d:%2.2d+%2.2d:%2.2d %4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9d-%2.2d:%2.2d+%2.2d:%2.2d %6d/%2.2d:%2.2d:%2.2d.%9.9d\n"
    		, now, zulu, juliet
    		, zyear, zmonth, zday, zhour, zminute, zsecond, ztick, 0, 0, 0, 0
    		, jyear, jmonth, jday, jhour, jminute, jsecond, jtick, zh, zm, dh, dm
    		, dday, dhour, dminute, dsecond, dtick
    	);
	}
	ASSERT(now == zulu);
	ASSERT(now == juliet);
	ASSERT((1901 <= zyear) && (zyear <= 2038));
	ASSERT((1 <= zmonth) && (zmonth <= 12));
	ASSERT((1 <= zday) && (zday <= 31));
	ASSERT((0 <= zhour) && (zhour <= 23));
	ASSERT((0 <= zminute) && (zminute <= 59));
	ASSERT((0 <= zsecond) && (zsecond <= 60)); /* To account for leap seconds. */
	ASSERT((0 <= ztick) && (ztick <= 999999));
	ASSERT((1901 <= jyear) && (jyear <= 2038));
	ASSERT((1 <= jmonth) && (jmonth <= 12));
	ASSERT((1 <= jday) && (jday <= 31));
	ASSERT((0 <= jhour) && (jhour <= 23));
	ASSERT((0 <= jminute) && (jminute <= 59));
	ASSERT((0 <= jsecond) && (jsecond <= 60)); /* To account for leap seconds. */
	ASSERT((0 <= jtick) && (jtick <= 999999));
	ASSERT((0 <= dhour) && (dhour <= 23));
	ASSERT((0 <= dminute) && (dminute <= 59));
	ASSERT((0 <= dsecond) && (dsecond <= 60)); /* To account for leap seconds. */
	ASSERT((0 <= dtick) && (dtick <= 999999));
	ASSERT((rc < 0) || (rc > 0));

	prior = zyear;
}

static const diminuto_ticks_t LOW = 0xffffffff80000000LL;
static const diminuto_ticks_t HIGH = 0x000000007fffffffLL - (7 * 3600) - 3600;

int main(int argc, char ** argv)
{
    diminuto_ticks_t now;
    diminuto_ticks_t hertz;

    SETLOGMASK();

    diminuto_core_enable();

    hertz = diminuto_time_frequency();

    DIMINUTO_LOG_INFORMATION("TEST 1\n");

    epoch(LOW * hertz, !0);
    epoch(-hertz, !0);
    epoch(0, !0);
    epoch(HIGH * hertz, !0);

    DIMINUTO_LOG_INFORMATION("TEST 2\n");

    for (now = LOW; now <= HIGH; now += (365 * 24 * 60 * 60)) {
    	epoch(now * hertz, 0);
     }

    DIMINUTO_LOG_INFORMATION("TEST 3\n");

    for (now = LOW; now <= HIGH; now += (24 * 60 * 60)) {
    	epoch(now * hertz, 0);
    }

    DIMINUTO_LOG_INFORMATION("TEST 4\n");

    for (now = LOW; now <= HIGH; now += (60 * 60)) {
    	epoch(now * hertz, 0);
    }

    DIMINUTO_LOG_INFORMATION("TEST 5\n");

    for (now = LOW; now <= HIGH; now += 60) {
    	epoch(now * hertz, 0);
    }

    DIMINUTO_LOG_INFORMATION("TEST 6\n");

    for (now = LOW; now <= HIGH; now += 1) {
    	epoch(now * hertz, 0);
    }

    EXIT();
}
