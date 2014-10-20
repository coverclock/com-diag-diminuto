/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
	EXPECT(diminuto_frequency_ticks2units(0LL, 1LL) == 0LL);
	EXPECT(diminuto_frequency_ticks2units(0LL, 1000LL) == 0LL);
	EXPECT(diminuto_frequency_ticks2units(0LL, 1000000LL) == 0LL);
	EXPECT(diminuto_frequency_ticks2units(0LL, 1000000000LL) == 0LL);
	EXPECT(diminuto_frequency_ticks2units(0LL, 10000000000LL) == 0LL);

	EXPECT(diminuto_frequency_ticks2units(diminuto_frequency(), 1LL) == 1LL);
	EXPECT(diminuto_frequency_ticks2units(diminuto_frequency(), 1000LL) == 1000LL);
	EXPECT(diminuto_frequency_ticks2units(diminuto_frequency(), 1000000LL) == 1000000LL);
	EXPECT(diminuto_frequency_ticks2units(diminuto_frequency(), 1000000000LL) == 1000000000LL);
	EXPECT(diminuto_frequency_ticks2units(diminuto_frequency(), 10000000000LL) == 10000000000LL);

	EXPECT(diminuto_frequency_units2ticks(0LL, 1LL) == 0LL);
	EXPECT(diminuto_frequency_units2ticks(0LL, 1000LL) == 0LL);
	EXPECT(diminuto_frequency_units2ticks(0LL, 1000000LL) == 0LL);
	EXPECT(diminuto_frequency_units2ticks(0LL, 1000000000LL) == 0LL);
	EXPECT(diminuto_frequency_units2ticks(0LL, 10000000000LL) == 0LL);

	EXPECT(diminuto_frequency_units2ticks(1LL, 1LL) == diminuto_frequency());
	EXPECT(diminuto_frequency_units2ticks(1000LL, 1000LL) == diminuto_frequency());
	EXPECT(diminuto_frequency_units2ticks(1000000LL, 1000000LL) == diminuto_frequency());
	EXPECT(diminuto_frequency_units2ticks(1000000000LL, 1000000000LL) == diminuto_frequency());
	EXPECT(diminuto_frequency_units2ticks(10000000000LL, 10000000000LL) == diminuto_frequency());

	EXPECT(diminuto_frequency_units2ticks(100000000000LL, 10000000000LL) == (diminuto_frequency() * 10));

	EXPECT(diminuto_frequency_ticks2wholeseconds(0) == 0);

	EXPECT(diminuto_frequency_ticks2wholeseconds(diminuto_frequency() * 1) == 1);
	EXPECT(diminuto_frequency_ticks2wholeseconds(diminuto_frequency() * 3) == 3);
	EXPECT(diminuto_frequency_ticks2wholeseconds(diminuto_frequency() * 5) == 5);

	EXPECT(diminuto_frequency_ticks2fractionalseconds(diminuto_frequency() - 1, diminuto_frequency()) == (diminuto_frequency() - 1));

	EXPECT(diminuto_frequency_seconds2ticks(0, 0, diminuto_frequency()) == 0);
	EXPECT(diminuto_frequency_seconds2ticks(1, 0, diminuto_frequency()) == diminuto_frequency());
	EXPECT(diminuto_frequency_seconds2ticks(2, diminuto_frequency() - 1, diminuto_frequency()) == ((diminuto_frequency() * 3) - 1));

	EXPECT(diminuto_frequency_rounddown(0LL, 1LL) == 0LL);
	EXPECT(diminuto_frequency_rounddown(1234567890LL, 1LL) == 1234567890LL);
	EXPECT(diminuto_frequency_rounddown(1234567890LL, 1000LL) == 1234567000LL);
	EXPECT(diminuto_frequency_rounddown(1234567890LL, 10000LL) == 1234560000LL);

	EXPECT(diminuto_frequency_roundup(0LL, 1LL) == 0LL);
	EXPECT(diminuto_frequency_roundup(1234567890LL, 1LL) == 1234567890LL);
	EXPECT(diminuto_frequency_roundup(1234567890LL, 1000LL) == 1234568000LL);
	EXPECT(diminuto_frequency_roundup(1234567890LL, 10000LL) == 1234570000LL);

	/*
	 * If we ever change the duration of a tick, these tests will have to be
	 * changed.
	 */

	EXPECT(diminuto_frequency() == 1000000000LL);

	EXPECT(diminuto_frequency_ticks2units(250LL, 10000000000LL) == 2500LL);
	EXPECT(diminuto_frequency_ticks2units(250LL, 1000000000LL) == 250LL);
	EXPECT(diminuto_frequency_ticks2units(250LL, 100000000LL) == 25LL);
	EXPECT(diminuto_frequency_ticks2units(250LL, 1000000LL) == 0LL);

	EXPECT(diminuto_frequency_units2ticks(250LL, 10000000000LL) == 25LL);
	EXPECT(diminuto_frequency_units2ticks(250LL, 1000000000LL) == 250LL);
	EXPECT(diminuto_frequency_units2ticks(250LL, 1000000LL) == 250000LL);
	EXPECT(diminuto_frequency_units2ticks(250LL, 1000LL) == 250000000LL);
	EXPECT(diminuto_frequency_units2ticks(250LL, 1LL) == 250000000000LL);

    EXIT();
}
