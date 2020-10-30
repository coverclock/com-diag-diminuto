/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Prints on stdout the UTC time in an ISO8601 timestamp.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * USAGE
 *
 * zulu<BR>
 *
 * EXAMPLES
 *
 * zulu<BR>
 *
 * ABSTRACT
 *
 * Prints on standard output the UTC time in an ISO8601 timestamp.
 */

#include <stdio.h>
#include <string.h>
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"

int main(int argc, char ** argv)
{
    diminuto_ticks_t ticks;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    diminuto_ticks_t fraction;

    if ((ticks = diminuto_time_clock()) < 0) {
        return 2;
    }

    if (diminuto_time_zulu(ticks, &year, &month, &day, &hour, &minute, &second, &fraction) < 0) {
        return 3;
    }

    printf("%4.4d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d.%9.9llu+00:00\n", year, month, day, hour, minute, second, (long long unsigned int)fraction);

    return 0;
}
