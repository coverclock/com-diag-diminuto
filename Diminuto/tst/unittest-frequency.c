/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    diminuto_ticks_t ticks;

    ticks = diminuto_frequency();
    EXPECT(ticks == diminuto_frequency()); /* Testing for truncation. */

    EXPECT(diminuto_frequency() == COM_DIAG_DIMINUTO_FREQUENCY);
    EXPECT(diminuto_time_frequency() == COM_DIAG_DIMINUTO_TIME_FREQUENCY);
    EXPECT(diminuto_timer_frequency() == COM_DIAG_DIMINUTO_TIMER_FREQUENCY);
    EXPECT(diminuto_delay_frequency() == COM_DIAG_DIMINUTO_DELAY_FREQUENCY);
    EXPECT(diminuto_mux_frequency() == COM_DIAG_DIMINUTO_MUX_FREQUENCY);

    EXPECT(diminuto_time_frequency() <= diminuto_frequency());
    EXPECT(diminuto_timer_frequency() <= diminuto_frequency());
    EXPECT(diminuto_delay_frequency() <= diminuto_frequency());
    EXPECT(diminuto_mux_frequency() <= diminuto_frequency());

    CHECKPOINT("f(ticks)=%lldHz T(ticks)=%lldticks\n", diminuto_frequency(), diminuto_frequency() / diminuto_frequency());
    CHECKPOINT("f(time)=%lldHz T(time)=%lldticks\n", diminuto_time_frequency(), diminuto_frequency() / diminuto_time_frequency());
    CHECKPOINT("f(timer)=%lldHz T(timer)=%lldticks\n", diminuto_timer_frequency(), diminuto_frequency() / diminuto_timer_frequency());
    CHECKPOINT("f(delay)=%lldHz T(delay)=%lldticks\n", diminuto_delay_frequency(), diminuto_frequency() / diminuto_delay_frequency());
    CHECKPOINT("f(mux)=%lldHz T(mux)=%lldticks\n", diminuto_mux_frequency(), diminuto_frequency() / diminuto_mux_frequency());

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

#if (COM_DIAG_DIMINUTO_FREQUENCY != 1000000000LL)
#   error Unit test must be updated to new resolution of ticks!
#endif

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
