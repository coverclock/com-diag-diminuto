/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Prints on stdout the fundamental base frequency.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * USAGE
 *
 * frequency
 *
 * ABSTRACT
 *
 * Prints on standard output the base frequency of the Diminuto library.
 * This is the number of ticks per second used for the basis of all time
 * measurement in the library. The duration of the basic tick unit is the
 * reciprocal of this number.
 */

#include <stdio.h>
#include "com/diag/diminuto/diminuto_frequency.h"

int main(void)
{
    printf("%lld\n", (long long int)diminuto_frequency());

    return 0;
}
