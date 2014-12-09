/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * WORK IN PROGRESS!
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_shaper.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <stdio.h>
#include <stdlib.h>

static const size_t BLOCKSIZE = 32768;

static inline size_t blocksize(void)
{
    return (rand() % BLOCKSIZE) + 1;
}

int main(int argc, char ** argv)
{
    SETLOGMASK();

    {
        static const size_t PEAK = 2048;
        static const diminuto_ticks_t TOLERANCE = 0;
        static const size_t SUSTAINED = 1024;
        static const size_t BURST = 32768;
        static const size_t OPERATIONS = 1000000;
        diminuto_shaper_t shaper;
        diminuto_shaper_t * sp;
        size_t size;
        diminuto_ticks_t now = 0;
        diminuto_ticks_t delay;
        size_t iops;
        int admissable;
        uint64_t total = 0;
        diminuto_ticks_t duration = 0;
        int64_t peak = 0;
        int64_t sustained;
        sp = &shaper;
        ASSERT(diminuto_shaper_init(sp, PEAK, TOLERANCE, SUSTAINED, BURST, now) == sp);
        srand(diminuto_time_clock());
        for (iops = 0; iops < OPERATIONS; ++iops) {
            delay = diminuto_shaper_request(sp, now);
            now += delay;
            duration += delay;
            if (duration > diminuto_frequency()) {
                sustained = total / (duration / diminuto_frequency());
                if (sustained > peak) {
                    peak = sustained;
                }
            }
            delay = diminuto_shaper_request(sp, now);
            ASSERT(delay == 0);
            size = blocksize();
            ASSERT(size > 0);
            ASSERT(size <= BLOCKSIZE);
            total += size;
            admissable = !diminuto_shaper_commitn(sp, size);
            ASSERT(admissable);
        }
        ASSERT(total > 0);
        ASSERT(duration > diminuto_frequency());
        sustained = total / (duration / diminuto_frequency());
        DIMINUTO_LOG_DEBUG("operations=%zu total=%zubytes average=%zubytes duration=%lldseconds peak=%zubytes/second measured=%lldbytes/second sustained=%zubytes/second measured=%lldbytes/second\n", iops, total, total / iops, duration / diminuto_frequency(), PEAK, peak, SUSTAINED, sustained);
        ASSERT(peak <= PEAK);
        ASSERT(llabs(sustained - SUSTAINED) < (SUSTAINED / 200));
     }

    EXIT();
}

