/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Alarm feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Alarm feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

extern int diminuto_alarm_debug;

int main(int argc, char ** argv)
{
    int rc = 0;

    SETLOGMASK();

    TEST();

    diminuto_alarm_debug = !0;

    ASSERT(diminuto_alarm_install(!0) == 0);
    ASSERT(!diminuto_alarm_check());

    ASSERT(diminuto_timer_periodic(diminuto_frequency()) == 0);

    COMMENT("delaying 10 seconds.");

    diminuto_delay(diminuto_frequency() * 10, 0);

    ASSERT(diminuto_timer_periodic(0) >= 0);

    ASSERT((rc = diminuto_alarm_check()) > 0);
    EXPECT(rc == 10);
    ASSERT(!diminuto_alarm_check());

    STATUS();

    EXIT();
}
