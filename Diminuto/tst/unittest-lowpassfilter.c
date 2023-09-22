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
#include <math.h>
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
        static const int LIMIT = 200000;

        TEST();

        ticks = diminuto_time_clock();
        ASSERT(ticks >= 0);
        srandom(ticks);
        minimum = MAXIMUM;
        lowest = MAXIMUM;

        if (argc > 1) {
            printf("%s, %s, %s\n", "COUNT", "RANDOM", "FILTERED");
        }

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
            "average=%lld\n"
            "middle=%lld\n",
            total,
            count,
            value,
            minimum,
            maximum,
            (total / count),
            (((long long)maximum + (long long)minimum) / 2),
            lowest,
            highest,
            (sum / count),
            (((long long)lowest + (long long)highest) / 2)
        );

        STATUS();
    }

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
        int ii;
        double x;
        double y;
        static const long MAXIMUM = diminuto_maximumof(long);
        static const int LIMIT = 200000;

        TEST();

        minimum = MAXIMUM;
        lowest = MAXIMUM;

        if (argc > 1) {
            printf("%s, %s, %s\n", "COUNT", "SINE", "FILTERED");
        }

        for (ii = 0; ii < LIMIT; ++ii) {
            x = M_PI * 2.0 * 10.0 * ii / LIMIT;
            y = sin(x);
            sample = y * 10000.0;
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
            "average=%lld\n"
            "middle=%lld\n",
            total,
            count,
            value,
            minimum,
            maximum,
            (total / count),
            (((long long)maximum + (long long)minimum) / 2),
            lowest,
            highest,
            (sum / count),
            (((long long)lowest + (long long)highest) / 2)
        );

        STATUS();
    }

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
        double x;
        double y;
        double noise = 0;
        static const long MAXIMUM = diminuto_maximumof(long);
        static const int LIMIT = 200000;

        TEST();

        ticks = diminuto_time_clock();
        ASSERT(ticks >= 0);
        srandom(ticks);
        minimum = MAXIMUM;
        lowest = MAXIMUM;

        if (argc > 1) {
            printf("%s, %s, %s\n", "COUNT", "NOISY", "FILTERED");
        }

        for (ii = 0; ii < LIMIT; ++ii) {
            noise = 1000.0 * (random() - (MAXIMUM / 2)) / MAXIMUM;
            x = M_PI * 2.0 * 10.0 * ii / LIMIT;
            y = sin(x);
            sample = (y * 10000.0) + noise;
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
            "average=%lld\n"
            "middle=%lld\n",
            total,
            count,
            value,
            minimum,
            maximum,
            (total / count),
            (((long long)maximum + (long long)minimum) / 2),
            lowest,
            highest,
            (sum / count),
            (((long long)lowest + (long long)highest) / 2)
        );

        STATUS();
    }

    EXIT();
}
