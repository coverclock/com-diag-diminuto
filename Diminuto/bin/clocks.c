/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Logs UTC (system), TAI, and elapsed clock times for comparison.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Logs Coordinated Universal Time or UTC (clock), International Atomic
 * Time or TAI, and elapsed clock time, for comparison. This is useful to
 * periodically emit to the log so that future clock offsets can be determined.
 * The messages is emitted unconditionally (regardless of the value of the
 * Log Mask), at the default log level (which at this time is NOTICE a.k.a.
 * NOTE).
 */

#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <string.h>

int main(int argc, char * argv[])
{
    const char * program = (const char *)0;
    diminuto_sticks_t frequency = 0;
    diminuto_sticks_t clock = 0;
    diminuto_sticks_t atomic = 0;
    diminuto_sticks_t elapsed = 0;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    frequency = diminuto_frequency();

    clock = diminuto_time_clock();
    atomic = diminuto_time_atomic();
    elapsed = diminuto_time_elapsed();

    diminuto_log_emit("%s: frequency=%lldHz clock=%lldticks atomic=%lldticks elapsed=%lldticks\n", program, (diminuto_lld_t)frequency, (diminuto_lld_t)clock, (diminuto_lld_t)atomic, (diminuto_lld_t)elapsed);
}
