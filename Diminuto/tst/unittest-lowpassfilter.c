/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the low pass filter macro.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * usage: unittest-lowtestfilter [ MASK ]
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_lowpassfilter.h"

int main(int argc, char ** argv)
{
    unsigned long mask = 0;
    static const long MAXIMUM = diminuto_maximumof(long);
    static const int LIMIT = 100;
    static const long RANGE = diminuto_maximumof(int32_t); /* random(3) */

    SETLOGMASK();

    {
        diminuto_sticks_t ticks = 0;
        char * end = (char *)0;

        TEST();

        ticks = diminuto_time_clock();
        ASSERT(ticks >= 0);
        srandom(ticks);

        if (argc < 2) {
            /* Do nothing. */
        } else if (argv[1] == (char *)0) {
            /* Do nothing. */
        } else {
            mask = strtoul(argv[1], &end, 0);
            ASSERT((end != (char *)0) && (*end == '\0'));
        }

        STATUS();
    }

    {
        int sample = 0;
        int accumulator = 0;
        int value = 0;
        int count = 0;

        TEST();

        ASSERT(sample == 0);
        ASSERT(accumulator == 0);
        ASSERT(value == 0);
        ASSERT(count == 0);

        value = DIMINUTO_LOWPASSFILTER(sample = 0, accumulator, count);

        ASSERT(sample == 0);
        ASSERT(accumulator == 0);
        ASSERT(value == 0);
        ASSERT(count == 1);

        value = DIMINUTO_LOWPASSFILTER(sample = 1, accumulator, count);

        ASSERT(sample == 1);
        ASSERT(accumulator == 0);
        ASSERT(value == 0);
        ASSERT(count == 2);

        value = DIMINUTO_LOWPASSFILTER(sample = 2, accumulator, count);

        ASSERT(sample == 2);
        ASSERT(accumulator == 1);
        ASSERT(value == 1);
        ASSERT(count == 3);

        value = DIMINUTO_LOWPASSFILTER(sample = 3, accumulator, count);

        ASSERT(sample == 3);
        ASSERT(accumulator == 2);
        ASSERT(value == 2);
        ASSERT(count == 4);

        STATUS();
    }

    {
        bool verbose = false;
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

        TEST();

        verbose = ((mask & (1 << 0)) != 0);

        minimum = MAXIMUM;
        lowest = MAXIMUM;

        if (verbose) {
            printf("%s, %s, %s\n", "COUNT", "RANDOM", "FILTERED");
        }

        for (ii = 0; ii < LIMIT; ++ii) {
            sample = random();
            ASSERT((0 <= sample) && (sample <= MAXIMUM));
            if (sample > maximum) { maximum = sample; }
            if (sample < minimum) { minimum = sample; }
            total += sample;
            value = DIMINUTO_LOWPASSFILTER(sample, accumulator, count);
            if (verbose) {
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
        bool verbose = false;
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

        TEST();

        verbose = ((mask & (1 << 1)) != 0);

        minimum = MAXIMUM;
        lowest = MAXIMUM;

        if (verbose) {
            printf("%s, %s, %s\n", "COUNT", "SINE", "FILTERED");
        }

        for (ii = 0; ii < LIMIT; ++ii) {
            x = M_PI;
            x *= 2.0;
            x *= 10.0;
            x /= LIMIT;
            y = sin(x);
            y *= 100.0;
            sample = y;
            if (sample > maximum) { maximum = sample; }
            if (sample < minimum) { minimum = sample; }
            total += sample;
            value = DIMINUTO_LOWPASSFILTER(sample, accumulator, count);
            if (verbose) {
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
        bool verbose = false;
        long signal = 0;
        long noise = 0;
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
        double z;

        TEST();

        verbose = ((mask & (1 << 2)) != 0);

        minimum = MAXIMUM;
        lowest = MAXIMUM;

        if (verbose) {
            printf("%s, %s, %s, %s, %s\n", "COUNT", "SIGNAL", "NOISE", "SAMPLE", "FILTERED");
        }

        for (ii = 0; ii < LIMIT; ++ii) {
            x = M_PI;
            x *= 2.0;
            x *= 10.0;
            x *= ii;
            x /= LIMIT;
            y = sin(x);
            y *= 100.0;
            signal = y;
            z = random();
            z /= RANGE;
            z -= 0.5;
            z *= 10.0;
            noise = z;
            sample = signal + noise;
            if (sample > maximum) { maximum = sample; }
            if (sample < minimum) { minimum = sample; }
            total += sample;
            value = DIMINUTO_LOWPASSFILTER(sample, accumulator, count);
            if (verbose) {
                printf("%zu, %ld, %ld, %ld, %ld\n", count, signal, noise, sample, value);
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
        bool verbose = false;
        double signal = 0;
        double noise = 0;
        double sample = 0;
        double accumulator = 0;
        double value = 0;
        double total = 0;
        double minimum = 0;
        double maximum = 0;
        double sum = 0;
        double highest = 0;
        double lowest = 0;
        size_t count = 0;
        int ii;
        double x;
        double y;
        double z;

        TEST();

        verbose = ((mask & (1 << 3)) != 0);

        minimum = MAXIMUM;
        lowest = MAXIMUM;

        if (verbose) {
            printf("%s, %s, %s, %s, %s\n", "COUNT", "SIGNAL", "NOISE", "SAMPLE", "FILTERED");
        }

        for (ii = 0; ii < LIMIT; ++ii) {
            x = M_PI;
            x *= 2.0;
            x *= 10.0;
            x *= ii;
            x /= LIMIT;
            y = sin(x);
            y *= 100.0;
            signal = y;
            z = random();
            z /= RANGE;
            z -= 0.5;
            z *= 10.0;
            noise = z;
            sample = signal + noise;
            if (sample > maximum) { maximum = sample; }
            if (sample < minimum) { minimum = sample; }
            total += sample;
            value = DIMINUTO_LOWPASSFILTER(sample, accumulator, count);
            if (verbose) {
                printf("%zu, %lf, %lf, %lf, %lf\n", count, signal, noise, sample, value);
            }
            sum += value;
            if (value > highest) { highest = value; }
            if (value < lowest) { lowest = value; }
        }

        ASSERT(count == LIMIT);

        fprintf(stderr,
            "total=%lf\n"
            "count=%zu\n"
            "value=%lf\n"
            "minimum=%lf\n"
            "maximum=%lf\n"
            "mean=%lf\n"
            "median=%lf\n"
            "lowest=%lf\n"
            "highest=%lf\n"
            "average=%lf\n"
            "middle=%lf\n",
            total,
            count,
            value,
            minimum,
            maximum,
            (total / count),
            ((maximum + minimum) / 2.0),
            lowest,
            highest,
            (sum / count),
            ((lowest + highest) / 2.0)
        );

        STATUS();
    }

    EXIT();
}
