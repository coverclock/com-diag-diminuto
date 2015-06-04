/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_throttle.h"
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
        static const diminuto_ticks_t II = 0x123456789abcdef0LL;
        static const diminuto_ticks_t LL = 0x9abcdef012345678LL;
        static const diminuto_ticks_t TT = 0xfedcba9876543210LL;
        diminuto_throttle_t throttle;
        diminuto_throttle_t * tp;
        /**/
        tp = diminuto_throttle_init(&throttle, II, LL, TT);
        ASSERT(tp == &throttle);
        ASSERT(throttle.increment == II);
        ASSERT(throttle.limit == LL);
        ASSERT(throttle.now == TT);
        ASSERT(throttle.then == (TT - II));
        ASSERT(throttle.expected == II);
        ASSERT(throttle.actual == 0);
        ASSERT(throttle.alarmed == 0);
        ASSERT(throttle.alarming == 0);
        ASSERT(throttle.alarmly == 0);
        ASSERT(throttle.cleared == !0);
        ASSERT(throttle.clearing == !0);
        ASSERT(throttle.clearly == !0);
        /**/
        throttle.now = ~throttle.now;
        throttle.then = ~throttle.then;
        throttle.expected = ~throttle.expected;
        throttle.actual = ~throttle.actual;
        throttle.alarmed = ~throttle.alarmed;
        throttle.alarming = ~throttle.alarming;
        throttle.alarmly = ~throttle.alarmly;
        throttle.cleared = ~throttle.cleared;
        throttle.clearing = ~throttle.clearing;
        throttle.clearly = ~throttle.clearly;
        ASSERT(throttle.increment == II);
        ASSERT(throttle.limit == LL);
        ASSERT(throttle.now != TT);
        ASSERT(throttle.then != (TT - II));
        ASSERT(throttle.expected != II);
        ASSERT(throttle.actual != 0);
        ASSERT(throttle.alarmed != 0);
        ASSERT(throttle.alarming != 0);
        ASSERT(throttle.alarmly != 0);
        ASSERT(throttle.cleared == 0);
        ASSERT(throttle.clearing == 0);
        ASSERT(throttle.clearly == 0);
        tp = diminuto_throttle_reset(&throttle, TT);
        ASSERT(tp == &throttle);
        ASSERT(throttle.increment == II);
        ASSERT(throttle.limit == LL);
        ASSERT(throttle.now == TT);
        ASSERT(throttle.then == (TT - II));
        ASSERT(throttle.expected == II);
        ASSERT(throttle.actual == 0);
        ASSERT(throttle.alarmed == 0);
        ASSERT(throttle.alarming == 0);
        ASSERT(throttle.alarmly == 0);
        ASSERT(throttle.cleared == !0);
        ASSERT(throttle.clearing == !0);
        ASSERT(throttle.clearly == !0);
    }

    {
        diminuto_throttle_t throttle;
        diminuto_throttle_t * tp;
        diminuto_ticks_t ii;
        diminuto_ticks_t ll;
        diminuto_ticks_t now;
        tp = &throttle;
        ii = 100;
        ll = 10;
        ASSERT(diminuto_throttle_init(tp, ii, ll, 0) == tp);
        /* SUSTAINED */
        now = 0;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        /* CONSUME LIMIT */
        now += ii - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        /* ALARM */
        now += ii - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        ASSERT(diminuto_throttle_commit(tp));
        now += ii + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        ASSERT(diminuto_throttle_commit(tp));
        now += ii + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        /* REQUEST, RE-REQUESTS, COMMIT */
        now += ii - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        /* REQUEST, DELAY, ADMIT */
        now += ii - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 2;
        ASSERT(!diminuto_throttle_admit(tp, now));
        /* SUSTAINED AGAIN */
        now += ii + 10;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += ii;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
    }

    {
        diminuto_throttle_t throttle;
        diminuto_throttle_t * tp;
        diminuto_ticks_t ii;
        diminuto_ticks_t ll;
        size_t size;
        diminuto_ticks_t now;
        tp = &throttle;
        ii = 100;
        ll = 10;
        ASSERT(diminuto_throttle_init(tp, ii, ll, 0) == tp);
        /* SUSTAINED */
        now = 0;
        size = 10;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        /* CONSUME LIMIT */
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        /* ALARM */
        now += (size * ii) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        ASSERT(diminuto_throttle_commitn(tp, size));
        now += (size * ii) + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        ASSERT(diminuto_throttle_commitn(tp, size));
        now += (size * ii) + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        /* REQUEST, RE-REQUESTS, COMMIT */
        now += (size * ii) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        /* REQUEST, DELAY, ADMIT */
        now += (size * ii) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 2;
        ASSERT(!diminuto_throttle_admitn(tp, now, size));
        /* SUSTAINED AGAIN */
        now += (size * ii) + 10;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
    }

    {
        diminuto_throttle_t throttle;
        diminuto_throttle_t * tp;
        diminuto_ticks_t ii;
        diminuto_ticks_t ll;
        size_t size;
        diminuto_ticks_t now;
        tp = &throttle;
        ii = 100;
        ll = 10;
        ASSERT(diminuto_throttle_init(tp, ii, ll, 0) == tp);
        /* SUSTAINED */
        now = 0;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        /* CONSUME LIMIT */
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        /* ALARM */
        now += (size * ii) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        ASSERT(diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii) + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        ASSERT(diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii) + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        /* REQUEST, RE-REQUESTS, COMMIT */
        now += (size * ii) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        /* REQUEST, DELAY, ADMIT */
        now += (size * ii) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 2;
        ASSERT(!diminuto_throttle_admitn(tp, now, (size = blocksize())));
        /* SUSTAINED AGAIN */
        now += (size * ii) + 10;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
        now += (size * ii);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize())));
    }

    {
        static const size_t BANDWIDTH = 1024;
        static const size_t OPERATIONS = 1000000;
        diminuto_throttle_t throttle;
        diminuto_throttle_t * tp;
        size_t size;
        diminuto_ticks_t now = 0;
        diminuto_ticks_t delay;
        size_t iops;
        int admissable;
        uint64_t total = 0;
        diminuto_ticks_t duration = 0;
        uint64_t measured;
        tp = diminuto_throttle_init(&throttle, diminuto_frequency() / BANDWIDTH, 0, now);
        ASSERT(tp == &throttle);
        diminuto_throttle_log(tp);
        srand(diminuto_time_clock());
        for (iops = 0; iops < OPERATIONS; ++iops) {
            delay = diminuto_throttle_request(tp, now);
            now += delay;
            duration += delay;
            delay = diminuto_throttle_request(tp, now);
            ASSERT(delay == 0);
            size = blocksize();
            ASSERT(size > 0);
            ASSERT(size <= BLOCKSIZE);
            total += size;
            admissable = !diminuto_throttle_commitn(tp, size);
            ASSERT(admissable);
        }
        ASSERT(total > 0);
        ASSERT(duration > diminuto_frequency());
        measured = total / (duration / diminuto_frequency());
        DIMINUTO_LOG_DEBUG("operations=%zu total=%llubytes average=%llubytes duration=%lldseconds requested=%zubytes/second measured=%lldbytes/second\n", iops, total, total / iops, duration / diminuto_frequency(), BANDWIDTH, measured);
        ASSERT(llabs(measured - BANDWIDTH) < (BANDWIDTH / 200));
     }

    EXIT();
}

