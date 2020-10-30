/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is supporting part of the Modulator functional test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This is supporting part of the Modulator functional test.
 */

#include "com/diag/diminuto/diminuto_countof.h"
#include <assert.h>
#include <stdio.h>

static const int PRIMES[] = {
    7, 5, 3, 2
};

int main(int argc, char * argv[])
{
    int duty;
    int index;
    int prime;
    int on;
    int off;
    float percentage;

    for (duty = 0; duty <= 100; ++duty) {

        on = duty;
        off = 100 - duty;

        for (index = 0; index < countof(PRIMES); ++index) {
            prime = PRIMES[index];
            while (((on / prime) > 0) && ((on % prime) == 0) && ((off / prime) > 0) && ((off % prime) == 0)) {
                on /= prime;
                off /= prime;
            }
        }

        assert((100 % (on + off)) == 0);

        if (on > 0) {
            percentage = on;
            percentage /= on + off;
            percentage *= 100;
        }

        printf("%s: duty=%d=(%d,%d)=%.2f=<%d,%d>\n", argv[0], duty, duty, 100 - duty, percentage, on, off);

    }

    return 0;
}
