/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Frequency feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Frequency feature.
 */

#include <unistd.h>
#include <asm-generic/param.h>
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_error.h"

diminuto_sticks_t diminuto_frequency_f(void)
{
    return diminuto_frequency();
}

diminuto_sticks_t diminuto_frequency_user(void)
{
    diminuto_sticks_t ticks = 0;

    ticks = sysconf(_SC_CLK_TCK);
    if (ticks < 0) {
        diminuto_perror("diminuto_frequency_user: sysconf");
    }

    return ticks;
}

diminuto_sticks_t diminuto_frequency_system(void)
{
    diminuto_sticks_t ticks = 0;

    ticks = HZ;
    if (ticks < 0) {
        diminuto_perror("diminuto_frequency_system: HZ");
    }

    return ticks;
}
