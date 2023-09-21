/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the low pass filter macro.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

#include <stdint.h>
#include <stdlib.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_lowpassfilter.h"

int main(int argc, char ** argv)
{
    SETLOGMASK();

    {
        long sample = 0;
        long long accumulator = 0;
        long value = 0;
        long long total = 0;
        long minimum = 0;
        long maximum = 0;
        long long sum = 0;
        long highest = 0;
        long lowest = 0;
        size_t count = 0;
        diminuto_sticks_t ticks = 0;
        int ii;
        static const long MAXIMUM = diminuto_maximumof(long);
        static const int LIMIT = 1000000;

        TEST();

        ticks = diminuto_time_clock();
        ASSERT(ticks >= 0);
        srandom(ticks);
        minimum = MAXIMUM;
        lowest = MAXIMUM;

        for (ii = 0; ii < LIMIT; ++ii) {
            sample = random();
            ASSERT((0 <= sample) && (sample <= MAXIMUM));
            if (sample > maximum) { maximum = sample; }
            if (sample < minimum) { minimum = sample; }
            total += sample;
            value = DIMINUTO_LOWPASSFILTER(sample, accumulator, count);
            if (argc > 1) {
                printf("%zu, %ld, %ld\n", count, sample, value);
            }
            sum += value;
            if (value > highest) { highest = value; }
            if (value < lowest) { lowest = value; }
        }

        ASSERT(count == LIMIT);

        fprintf(stderr,
            "total=%lld\n"
            "count=%zu\n"
            "value=%ld\n"
            "minimum=%ld\n"
            "maximum=%ld\n"
            "mean=%lld\n"
            "median=%lld\n"
            "lowest=%ld\n"
            "highest=%ld\n"
            "average=%lld\n",
            total,
            count,
            value,
            minimum,
            maximum,
            (total / count),
            (((long long)maximum + (long long)minimum) / 2),
            lowest,
            highest,
            (sum / count)
        );

        STATUS();
    }

    EXIT();
}
