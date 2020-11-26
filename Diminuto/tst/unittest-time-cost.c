/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2013-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Time feature Cost.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Time feature Cost.
 *
 * USAGE
 *
 * unittest-time-cost [ iterations [ monotonic ] ]
 *
 * EXAMPLES
 *
 * unittest-time-cost 1000000000
 *
 * unittest-time-cost 1000000000 monotonic
 *
 * This program measures (approximately) the cost of reading the system clock.
 * It can be used to read either the real-time clock (wall clock) or the
 * monotonic clock (elapsed time). Why? Because it is quite common to want to
 * either time stamp or measure duration in real-time code, but you had better
 * have a grip on the cost of doing so. Also,if you measure the cost of reading
 * the real-time clock instead of the monotonic clock, be aware that changing
 * the system clock while this is running will invalidate the results.
 * Remarkably, on one client's Xilinx Zynq ZC702 ARM processor, reading the
 * monotonic clock was measurably and consistently more expensive than reading
 * the real-time clock. I didn't have time to dig into that project's 3.0 kernel
 * code to see why, but it's an unexpected (to me anyway) result. On my i686
 * 3.2 system, the two results are close enough to be within the margin of
 * measurement error. Reading the system clock on Linux from user space requires
 * a context switch into and out of kernel space. Compare this with reading
 * a monotonic clock on some non-Linux embedded systems, which may require just
 * reading a memory mapped time base register and so only takes a handful of
 * machine instructions.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdlib.h>
#include <errno.h>
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_time.h"

int main(int argc, char ** argv)
{
    int64_t limit = 1000000000ULL;
    int64_t iterations;
    diminuto_sticks_t before;
    diminuto_sticks_t after;
    diminuto_sticks_t total = 0;
    typedef diminuto_sticks_t (function_t)(void);
    function_t * functionp = &diminuto_time_clock;
    const char * label = "realtime";

    SETLOGMASK();

    if (argc > 1) {
        limit = strtoll(argv[1], (char **)0, 0);
    }

    if (argc > 2) {
        functionp = &diminuto_time_elapsed;
        label = "monotonic";
    }

    for (iterations = 0; iterations < limit; ++iterations) {
        after = (*functionp)();
        if (after < 0) {
            break;
        }
        if (iterations > 0) {
            total += (after - before);
        }
        before = after;
    }

    if (iterations <= 0) {
        errno = EINVAL;
        diminuto_perror(label);
        return 1;
    }

    CHECKPOINT("%s: %lldns / %llu = %lluns\n", label, total, iterations, (total + (iterations / 2)) / iterations);

    if (after < 0) {
        diminuto_perror(label);
        return 2;
    }

    EXIT();
}
