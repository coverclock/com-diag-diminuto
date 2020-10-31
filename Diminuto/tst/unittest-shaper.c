/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Shaper feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Shaper feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_shaper.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static inline size_t blocksize(size_t maximum)
{
    return (rand() % maximum) + 1;
}

int main(int argc, char ** argv)
{
    SETLOGMASK();

    {
        TEST();

        ASSERT(diminuto_shaper_bursttolerance(2, 3, 7, 5) == 23);
        ASSERT(diminuto_shaper_bursttolerance(2, 0, 7, 5) == 20);
        ASSERT(diminuto_shaper_bursttolerance(0, 0, 2, 5) == 8);
        ASSERT(diminuto_shaper_bursttolerance(7, 3, 7, 5) == 3);
        ASSERT(diminuto_shaper_bursttolerance(2, 3, 7, 1) == 3);
        ASSERT(diminuto_shaper_bursttolerance(2, 3, 7, 0) == 3);

        STATUS();
    }

    {
        static const size_t PEAK = 2048;
        static const diminuto_ticks_t TOLERANCE = 0;
        static const size_t SUSTAINED = 1024;
        static const size_t BURST = 512;
        static const size_t OPERATIONS = 1000000;
        diminuto_shaper_t shaper;
        diminuto_shaper_t * sp;
        size_t size = 0;
        diminuto_ticks_t now = 0;
        diminuto_ticks_t delay;
        size_t iops;
        int admissable;
        uint64_t total = 0;
        diminuto_ticks_t duration = 0;
        double peak = 0.0;
        double sustained;
        double rate;
        diminuto_ticks_t frequency;
        diminuto_ticks_t peakincrement;
        diminuto_ticks_t jittertolerance;
        diminuto_ticks_t sustainedincrement;
        diminuto_ticks_t bursttolerance;
        /**/
        frequency = diminuto_frequency();
        /**/
        peakincrement = diminuto_throttle_interarrivaltime(PEAK, 1, frequency);
        jittertolerance = diminuto_throttle_jittertolerance(peakincrement, BURST) + TOLERANCE;
        sustainedincrement = diminuto_throttle_interarrivaltime(SUSTAINED, 1, frequency);
        bursttolerance = diminuto_shaper_bursttolerance(peakincrement, jittertolerance, sustainedincrement, BURST);
        /**/
        sp = diminuto_shaper_init(&shaper, peakincrement, 0 /* jittertolerance */, sustainedincrement, bursttolerance, now);
        ASSERT(sp == &shaper);
        diminuto_shaper_log(sp);
        srand(diminuto_time_clock());
        /**/
        for (iops = 0; iops < OPERATIONS; ++iops) {
            delay = diminuto_shaper_request(sp, now);
            ASSERT(delay >= 0);
            now += delay;
            duration += delay;
            if (iops <= 0) {
                /* Do nothing. */
            } else if (delay <= 0) {
                /* Do nothing. */
            } else {
                rate = size;
                rate *= frequency;
                rate /= delay;
                if (rate > peak) { peak = rate; }
            }
            delay = diminuto_shaper_request(sp, now);
            ASSERT(delay == 0);
            size = blocksize(BURST);
            ASSERT(size > 0);
            ASSERT(size <= BURST);
            total += size;
            admissable = !diminuto_shaper_commitn(sp, size);
            ASSERT(admissable);
        }
        /**/
        delay = diminuto_shaper_getexpected(sp);
        ASSERT(delay >= 0);
        now += delay;
        duration += delay;
        if (delay > 0) {
            rate = size;
            rate *= frequency;
            rate /= delay;
            if (rate > peak) { peak = rate; }
        }
        diminuto_shaper_update(sp, now);
        /**/
        ASSERT(total > 0);
        ASSERT(duration > frequency);
        diminuto_shaper_log(sp);
        sustained = total;
        sustained *= frequency;
        sustained /= duration;
        DIMINUTO_LOG_DEBUG("operations=%zu total=%llubytes average=%llubytes duration=%lldseconds peak=%zubytes/second measured=%lfbytes/second sustained=%zubytes/second measured=%lfdbytes/second\n", iops, total, total / iops, duration / diminuto_frequency(), PEAK, peak, SUSTAINED, sustained);
        ASSERT(fabs(sustained - SUSTAINED) < (SUSTAINED / 200) /* 0.5% */);
        ADVISE(fabs(peak - PEAK) < (PEAK / 200) /* 0.5% */);
        ASSERT(diminuto_shaper_fini(&shaper) == (diminuto_shaper_t *)0);
     }

    EXIT();
}

