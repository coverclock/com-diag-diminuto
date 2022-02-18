/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Meter feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Meter feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_meter.h"
#include "com/diag/diminuto/diminuto_shaper.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_types.h"
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

        ASSERT(diminuto_meter_now() >= 0);

        STATUS();
    }

    {
        diminuto_meter_t meter;

        TEST();

        ASSERT(diminuto_meter_init(&meter, 0) == &meter); 

        ASSERT(diminuto_meter_events(&meter, DIMINUTO_TIME_ERROR, 0) < 0);
        ASSERT(errno == EINVAL);

        ASSERT(diminuto_meter_events(&meter, 0, 0) == 0);

        ASSERT(diminuto_meter_events(&meter, 1, 0) == 0);

        ASSERT(diminuto_meter_events(&meter, 0, 0) < 0);
        ASSERT(errno == ERANGE);

        ASSERT(diminuto_meter_events(&meter, 1, 1) == 0);

        ASSERT(diminuto_meter_events(&meter, 2, maximumof(typeof(meter.events))) < 0);
        ASSERT(errno == EOVERFLOW);

        ASSERT(diminuto_meter_events(&meter, 3, 1) < 0);
        ASSERT(errno == EOVERFLOW);

        ASSERT(diminuto_meter_fini(&meter) == (diminuto_meter_t *)0); 

        STATUS();
    }

    {
        static const size_t PEAK = 2048;
        static const diminuto_ticks_t TOLERANCE = 0;
        static const size_t SUSTAINED = 1024;
        static const size_t BURST = 512;
        static const size_t OPERATIONS = 1000000;
        diminuto_meter_t meter;
        diminuto_meter_t * mp;
        diminuto_shaper_t shaper;
        diminuto_shaper_t * sp;
        size_t size = 0;
        diminuto_ticks_t now = 0;
        diminuto_ticks_t delay;
        size_t iops;
        int admissable;
        uint64_t total = 0;
        diminuto_ticks_t duration = 0;
        diminuto_ticks_t frequency;
        diminuto_ticks_t peakincrement;
        diminuto_ticks_t jittertolerance;
        diminuto_ticks_t sustainedincrement;
        diminuto_ticks_t bursttolerance;
        double peak;
        double sustained;
        int rc;

        TEST();

        frequency = diminuto_frequency();

        peakincrement = diminuto_throttle_interarrivaltime(PEAK, 1, frequency);
        jittertolerance = diminuto_throttle_jittertolerance(peakincrement, BURST) + TOLERANCE;
        sustainedincrement = diminuto_throttle_interarrivaltime(SUSTAINED, 1, frequency);
        bursttolerance = diminuto_shaper_bursttolerance(peakincrement, jittertolerance, sustainedincrement, BURST);

        mp = diminuto_meter_init(&meter, now);
        ASSERT(mp == &meter);

        sp = diminuto_shaper_init(&shaper, peakincrement, 0 /* jittertolerance */, sustainedincrement, bursttolerance, now);
        ASSERT(sp == &shaper);
        diminuto_shaper_log(sp);

        srand(diminuto_time_clock());

        for (iops = 0; iops < OPERATIONS; ++iops) {
            delay = diminuto_shaper_request(sp, now);
            ASSERT(delay >= 0);
            /*
             * The delay reflects the time it took to transmit the *previous*
             * payload, not the one whose size we compute below.
             */
            now += delay;
            rc = diminuto_meter_events(mp, now, size);
            if (rc < 0) { diminuto_perror("diminuto_meter_events"); }
            ASSERT(rc >= 0);
            duration += delay;
            delay = diminuto_shaper_request(sp, now);
            ASSERT(delay == 0);
            size = blocksize(BURST);
            ASSERT(size > 0);
            ASSERT(size <= BURST);
            total += size;
            admissable = !diminuto_shaper_commitn(sp, size);
            ASSERT(admissable);
        }

        delay = diminuto_shaper_getexpected(sp);
        ASSERT(delay >= 0);
        now += delay;
        duration += delay;
        diminuto_shaper_update(sp, now);

        peak = diminuto_meter_peak(mp);
        ASSERT(peak != HUGE_VAL);
        sustained = diminuto_meter_sustained(mp);
        ASSERT(sustained != HUGE_VAL);

        size = diminuto_meter_burst(mp);
        ASSERT(size >= 0);

        ASSERT(total > 0);
        ASSERT(duration > frequency);

        diminuto_shaper_log(sp);

        CHECKPOINT("operations=%zu total=%llubytes average=%llubytes burst=%llubytes duration=%lluseconds peak=%zubytes/second measured=%lfbytes/second sustained=%zubytes/second measured=%lfdbytes/second\n", iops, (diminuto_llu_t)total, (diminuto_llu_t)(total / iops), (diminuto_llu_t)size, (diminuto_llu_t)(duration / diminuto_frequency()), PEAK, peak, SUSTAINED, sustained);

        EXPECT(fabs(sustained - SUSTAINED) < (SUSTAINED / 200) /* 0.5% */);
        EXPECT(fabs(peak - PEAK) < (PEAK / 200) /* 0.5% */);
        EXPECT(size <= BURST);

        ASSERT(diminuto_shaper_fini(&shaper) == (diminuto_shaper_t *)0);

        STATUS();
     }

    EXIT();
}
