/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Prints on stdout the epoch time or the time of day in the specified units since the POSIX epoch.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * USAGE
 *
 * epochtime [ UNITS ]<BR>
 *
 * EXAMPLES
 *
 * epochtime<BR>
 * epochtime 1<BR>
 * epochtime 1000<BR>
 * epochtime 1000000<BR>
 * epochtime 1000000000<BR>
 *
 * ABSTRACT
 *
 * Prints on standard output the epoch time or the time of day in
 * the specified units since the POSIX epoch. Defaults to seconds.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"

int main(int argc, char ** argv)
{
    diminuto_sticks_t ticks;
    diminuto_sticks_t units;
    char * endptr = 0;

    if (argc < 2) {
        units = 1;
    } else if ((units = strtoll(argv[1], &endptr, 0)) <= 0) {
        errno = EINVAL;
        diminuto_perror(argv[1]);
        return 1;
    } else if ((endptr == (char *)0) || (*endptr != '\0')) {
        errno = EINVAL;
        diminuto_perror(argv[1]);
        return 1;
    } else {
        /* Do nothing. */
    }

    if ((ticks = diminuto_time_clock()) == 0) {
        return 2;
    }

    printf("%lld\n", (long long int)diminuto_frequency_ticks2units(ticks, units));

    return 0;
}
