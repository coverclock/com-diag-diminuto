/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2013-2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Frequency feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Frequency feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_condition.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_meter.h"
#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_poll.h"
#include "com/diag/diminuto/diminuto_shaper.h"
#include "com/diag/diminuto/diminuto_throttle.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    SETLOGMASK();

    {
        diminuto_ticks_t ticks;
        diminuto_sticks_t sticks;
        TEST();
        ticks = diminuto_frequency();
        EXPECT(ticks == diminuto_frequency()); /* Testing for truncation. */
        sticks = diminuto_frequency();
        EXPECT(sticks == diminuto_frequency()); /* Testing for truncation. */
        EXPECT(ticks == sticks);
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency() == COM_DIAG_DIMINUTO_FREQUENCY);
        EXPECT(diminuto_condition_frequency() == COM_DIAG_DIMINUTO_CONDITION_FREQUENCY);
        EXPECT(diminuto_delay_frequency() == COM_DIAG_DIMINUTO_DELAY_FREQUENCY);
        EXPECT(diminuto_meter_frequency() == COM_DIAG_DIMINUTO_METER_FREQUENCY);
        EXPECT(diminuto_modulator_frequency() == COM_DIAG_DIMINUTO_MODULATOR_FREQUENCY);
        EXPECT(diminuto_mux_frequency() == COM_DIAG_DIMINUTO_MUX_FREQUENCY);
        EXPECT(diminuto_poll_frequency() == COM_DIAG_DIMINUTO_POLL_FREQUENCY);
        EXPECT(diminuto_shaper_frequency() == COM_DIAG_DIMINUTO_SHAPER_FREQUENCY);
        EXPECT(diminuto_throttle_frequency() == COM_DIAG_DIMINUTO_THROTTLE_FREQUENCY);
        EXPECT(diminuto_time_frequency() == COM_DIAG_DIMINUTO_TIME_FREQUENCY);
        EXPECT(diminuto_timer_frequency() == COM_DIAG_DIMINUTO_TIMER_FREQUENCY);
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency() <= diminuto_frequency());
        EXPECT(diminuto_condition_frequency() <= diminuto_frequency());
        EXPECT(diminuto_delay_frequency() <= diminuto_frequency());
        EXPECT(diminuto_meter_frequency() <= diminuto_frequency());
        EXPECT(diminuto_modulator_frequency() <= diminuto_frequency());
        EXPECT(diminuto_mux_frequency() <= diminuto_frequency());
        EXPECT(diminuto_poll_frequency() <= diminuto_frequency());
        EXPECT(diminuto_shaper_frequency() <= diminuto_frequency());
        EXPECT(diminuto_throttle_frequency() <= diminuto_frequency());
        EXPECT(diminuto_time_frequency() <= diminuto_frequency());
        EXPECT(diminuto_timer_frequency() <= diminuto_frequency());
        EXPECT(diminuto_timer_frequency() <= diminuto_frequency());
        STATUS();
    }

    {
        TEST();
        CHECKPOINT("f(frequency)=%lldHz p(frequency)=%lldticks\n", (long long int)diminuto_frequency(), (long long int)(diminuto_frequency() / diminuto_frequency()));
        CHECKPOINT("f(condition)=%lldHz p(condition)=%lldticks\n", (long long int)diminuto_condition_frequency(), (long long int)(diminuto_frequency() / diminuto_condition_frequency()));
        CHECKPOINT("f(delay)=%lldHz p(delay)=%lldticks\n", (long long int)diminuto_delay_frequency(), (long long int)(diminuto_frequency() / diminuto_delay_frequency()));
        CHECKPOINT("f(meter)=%lldHz p(meter)=%lldticks\n", (long long int)diminuto_meter_frequency(), (long long int)(diminuto_frequency() / diminuto_meter_frequency()));
        CHECKPOINT("f(modulator)=%lldHz p(modulator)=%lldticks\n", (long long int)diminuto_modulator_frequency(), (long long int)(diminuto_frequency() / diminuto_modulator_frequency()));
        CHECKPOINT("f(mux)=%lldHz p(mux)=%lldticks\n", (long long int)diminuto_mux_frequency(), (long long int)(diminuto_frequency() / diminuto_mux_frequency()));
        CHECKPOINT("f(poll)=%lldHz p(poll)=%lldticks\n", (long long int)diminuto_poll_frequency(), (long long int)(diminuto_frequency() / diminuto_poll_frequency()));
        CHECKPOINT("f(shaper)=%lldHz p(shaper)=%lldticks\n", (long long int)diminuto_shaper_frequency(), (long long int)(diminuto_frequency() / diminuto_shaper_frequency()));
        CHECKPOINT("f(throttle)=%lldHz p(throttle)=%lldticks\n", (long long int)diminuto_throttle_frequency(), (long long int)(diminuto_frequency() / diminuto_throttle_frequency()));
        CHECKPOINT("f(time)=%lldHz p(time)=%lldticks\n", (long long int)diminuto_time_frequency(), (long long int)(diminuto_frequency() / diminuto_time_frequency()));
        CHECKPOINT("f(timer)=%lldHz p(timer)=%lldticks\n", (long long int)diminuto_timer_frequency(), (long long int)(diminuto_frequency() / diminuto_timer_frequency()));
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency_ticks2units(0LL, 1LL) == 0LL);
        EXPECT(diminuto_frequency_ticks2units(0LL, 1000LL) == 0LL);
        EXPECT(diminuto_frequency_ticks2units(0LL, 1000000LL) == 0LL);
        EXPECT(diminuto_frequency_ticks2units(0LL, 1000000000LL) == 0LL);
        EXPECT(diminuto_frequency_ticks2units(0LL, 10000000000LL) == 0LL);
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency_ticks2units(diminuto_frequency(), 1LL) == 1LL);
        EXPECT(diminuto_frequency_ticks2units(diminuto_frequency(), 1000LL) == 1000LL);
        EXPECT(diminuto_frequency_ticks2units(diminuto_frequency(), 1000000LL) == 1000000LL);
        EXPECT(diminuto_frequency_ticks2units(diminuto_frequency(), 1000000000LL) == 1000000000LL);
        EXPECT(diminuto_frequency_ticks2units(diminuto_frequency(), 10000000000LL) == 10000000000LL);
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency_units2ticks(0LL, 1LL) == 0LL);
        EXPECT(diminuto_frequency_units2ticks(0LL, 1000LL) == 0LL);
        EXPECT(diminuto_frequency_units2ticks(0LL, 1000000LL) == 0LL);
        EXPECT(diminuto_frequency_units2ticks(0LL, 1000000000LL) == 0LL);
        EXPECT(diminuto_frequency_units2ticks(0LL, 10000000000LL) == 0LL);
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency_units2ticks(1LL, 1LL) == diminuto_frequency());
        EXPECT(diminuto_frequency_units2ticks(1000LL, 1000LL) == diminuto_frequency());
        EXPECT(diminuto_frequency_units2ticks(1000000LL, 1000000LL) == diminuto_frequency());
        EXPECT(diminuto_frequency_units2ticks(1000000000LL, 1000000000LL) == diminuto_frequency());
        EXPECT(diminuto_frequency_units2ticks(10000000000LL, 10000000000LL) == diminuto_frequency());
        EXPECT(diminuto_frequency_units2ticks(100000000000LL, 10000000000LL) == (diminuto_frequency() * 10));
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency_ticks2wholeseconds(0) == 0);
        EXPECT(diminuto_frequency_ticks2wholeseconds(diminuto_frequency() * 1) == 1);
        EXPECT(diminuto_frequency_ticks2wholeseconds(diminuto_frequency() * 3) == 3);
        EXPECT(diminuto_frequency_ticks2wholeseconds(diminuto_frequency() * 5) == 5);
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency_ticks2fractionalseconds(diminuto_frequency() - 1, diminuto_frequency()) == (diminuto_frequency() - 1));
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency_seconds2ticks(0, 0, diminuto_frequency()) == 0);
        EXPECT(diminuto_frequency_seconds2ticks(1, 0, diminuto_frequency()) == diminuto_frequency());
        EXPECT(diminuto_frequency_seconds2ticks(2, diminuto_frequency() - 1, diminuto_frequency()) == ((diminuto_frequency() * 3) - 1));
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency_rounddown(0LL, 1LL) == 0LL);
        EXPECT(diminuto_frequency_rounddown(1234567890LL, 1LL) == 1234567890LL);
        EXPECT(diminuto_frequency_rounddown(1234567890LL, 1000LL) == 1234567000LL);
        EXPECT(diminuto_frequency_rounddown(1234567890LL, 10000LL) == 1234560000LL);
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency_roundup(0LL, 1LL) == 0LL);
        EXPECT(diminuto_frequency_roundup(1234567890LL, 1LL) == 1234567890LL);
        EXPECT(diminuto_frequency_roundup(1234567890LL, 1000LL) == 1234568000LL);
        EXPECT(diminuto_frequency_roundup(1234567890LL, 10000LL) == 1234570000LL);
        STATUS();
    }

#if (COM_DIAG_DIMINUTO_FREQUENCY != 1000000000LL)
#   error Unit test must be updated to new resolution of ticks!
#endif

    {
        TEST();
        EXPECT(diminuto_frequency_ticks2units(250LL, 10000000000LL) == 2500LL);
        EXPECT(diminuto_frequency_ticks2units(250LL, 1000000000LL) == 250LL);
        EXPECT(diminuto_frequency_ticks2units(250LL, 100000000LL) == 25LL);
        EXPECT(diminuto_frequency_ticks2units(250LL, 1000000LL) == 0LL);
        STATUS();
    }

    {
        TEST();
        EXPECT(diminuto_frequency_units2ticks(250LL, 10000000000LL) == 25LL);
        EXPECT(diminuto_frequency_units2ticks(250LL, 1000000000LL) == 250LL);
        EXPECT(diminuto_frequency_units2ticks(250LL, 1000000LL) == 250000LL);
        EXPECT(diminuto_frequency_units2ticks(250LL, 1000LL) == 250000000LL);
        EXPECT(diminuto_frequency_units2ticks(250LL, 1LL) == 250000000000LL);
        STATUS();
    }

    EXIT();
}
