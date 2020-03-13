/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_throttle.h"
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

        ASSERT(((size_t)(~(size_t)0)) > 0);
        ASSERT(sizeof(diminuto_ticks_t) == 8);
        ASSERT(((diminuto_ticks_t)(~(diminuto_ticks_t)0)) > 0);

        STATUS();
    }

    {
        TEST();

        ASSERT(diminuto_throttle_interarrivaltime(2, 1, 4) == 2);
        ASSERT(diminuto_throttle_interarrivaltime(1, 2, 4) == 8);
        ASSERT(diminuto_throttle_interarrivaltime(2, 1, 5) == 3);
        ASSERT(diminuto_throttle_interarrivaltime(1, 2, 5) == 10);

        STATUS();
    }

    {
        TEST();

        ASSERT(diminuto_throttle_jittertolerance(2, 3) == 4);
        ASSERT(diminuto_throttle_jittertolerance(2, 0) == 0);
        ASSERT(diminuto_throttle_jittertolerance(2, 1) == 0);
        ASSERT(diminuto_throttle_jittertolerance(3, 2) == 3);

        STATUS();
    }

    {
        static const diminuto_ticks_t II = 0x123456789abcdef0LL;
        static const diminuto_ticks_t LL = 0x9abcdef012345678LL;
        static const diminuto_ticks_t TT = 0xfedcba9876543210LL;
        diminuto_throttle_t throttle;
        diminuto_throttle_t * tp;
        /**/
        TEST();
        /**/
        tp = diminuto_throttle_init(&throttle, II, LL, TT);
        ASSERT(tp == &throttle);
        ASSERT(throttle.increment == II);
        ASSERT(throttle.limit == LL);
        ASSERT(throttle.now == TT);
        ASSERT(throttle.then == (TT - II));
        ASSERT(throttle.expected == 0);
        ASSERT(throttle.actual == 0);
        ASSERT(throttle.full0 == 0);
        ASSERT(throttle.full1 == 0);
        ASSERT(throttle.full2 == 0);
        ASSERT(throttle.empty0 == !0);
        ASSERT(throttle.empty1 == !0);
        ASSERT(throttle.empty2 == !0);
        ASSERT(throttle.alarmed1 == 0);
        ASSERT(throttle.alarmed2 == 0);
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /**/
        throttle.now = ~throttle.now;
        throttle.then = ~throttle.then;
        throttle.expected = ~throttle.expected;
        throttle.actual = ~throttle.actual;
        throttle.full1 = ~throttle.full1;
        throttle.full0 = ~throttle.full0;
        throttle.full2 = ~throttle.full2;
        throttle.empty0 = ~throttle.empty0;
        throttle.empty1 = ~throttle.empty1;
        throttle.empty2 = ~throttle.empty2;
        throttle.alarmed1 = ~throttle.alarmed1;
        throttle.alarmed2 = ~throttle.alarmed2;
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        tp = diminuto_throttle_init(&throttle, II, LL, TT);
        ASSERT(tp == &throttle);
        ASSERT(throttle.increment == II);
        ASSERT(throttle.limit == LL);
        ASSERT(throttle.now == TT);
        ASSERT(throttle.then == (TT - II));
        ASSERT(throttle.expected == 0);
        ASSERT(throttle.actual == 0);
        ASSERT(throttle.full0 == 0);
        ASSERT(throttle.full1 == 0);
        ASSERT(throttle.full2 == 0);
        ASSERT(throttle.empty0 == !0);
        ASSERT(throttle.empty1 == !0);
        ASSERT(throttle.empty2 == !0);
        ASSERT(throttle.alarmed1 == 0);
        ASSERT(throttle.alarmed2 == 0);
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /**/
        throttle.now = ~throttle.now;
        throttle.then = ~throttle.then;
        throttle.expected = ~throttle.expected;
        throttle.actual = ~throttle.actual;
        throttle.full1 = ~throttle.full1;
        throttle.full0 = ~throttle.full0;
        throttle.full2 = ~throttle.full2;
        throttle.empty0 = ~throttle.empty0;
        throttle.empty1 = ~throttle.empty1;
        throttle.empty2 = ~throttle.empty2;
        throttle.alarmed1 = ~throttle.alarmed1;
        throttle.alarmed2 = ~throttle.alarmed2;
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        tp = diminuto_throttle_reset(&throttle, TT);
        ASSERT(tp == &throttle);
        ASSERT(throttle.increment == II);
        ASSERT(throttle.limit == LL);
        ASSERT(throttle.now == TT);
        ASSERT(throttle.then == (TT - II));
        ASSERT(throttle.expected == 0);
        ASSERT(throttle.actual == 0);
        ASSERT(throttle.full0 == 0);
        ASSERT(throttle.full1 == 0);
        ASSERT(throttle.full2 == 0);
        ASSERT(throttle.empty0 == !0);
        ASSERT(throttle.empty1 == !0);
        ASSERT(throttle.empty2 == !0);
        ASSERT(throttle.alarmed1 == 0);
        ASSERT(throttle.alarmed2 == 0);
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /**/
        STATUS();
    }

    {
        diminuto_throttle_t throttle;
        diminuto_throttle_t * tp;
        diminuto_ticks_t increment;
        diminuto_ticks_t limit;
        diminuto_ticks_t now;
        /**/
        TEST();
        /**/
        tp = &throttle;
        increment = 100;
        limit = 10;
        ASSERT(diminuto_throttle_init(tp, increment, limit, 0) == tp);
        /**/
        ASSERT(diminuto_throttle_isempty(tp));
        ASSERT(!diminuto_throttle_isfull(tp));
        ASSERT(!diminuto_throttle_isalarmed(tp));
        ASSERT(!diminuto_throttle_emptied(tp));
        ASSERT(!diminuto_throttle_filled(tp));
        ASSERT(!diminuto_throttle_alarmed(tp));
        ASSERT(!diminuto_throttle_cleared(tp));
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /* SUSTAINED */
        now = 0;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        /**/
        ASSERT(diminuto_throttle_isempty(tp));
        ASSERT(!diminuto_throttle_isfull(tp));
        ASSERT(!diminuto_throttle_isalarmed(tp));
        ASSERT(!diminuto_throttle_emptied(tp));
        ASSERT(!diminuto_throttle_filled(tp));
        ASSERT(!diminuto_throttle_alarmed(tp));
        ASSERT(!diminuto_throttle_cleared(tp));
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /* CONSUME LIMIT */
        now += increment - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        /**/
        ASSERT(!diminuto_throttle_isempty(tp));
        ASSERT(!diminuto_throttle_isfull(tp));
        ASSERT(!diminuto_throttle_isalarmed(tp));
        ASSERT(!diminuto_throttle_emptied(tp));
        ASSERT(!diminuto_throttle_filled(tp));
        ASSERT(!diminuto_throttle_alarmed(tp));
        ASSERT(!diminuto_throttle_cleared(tp));
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /* FILL */
        now += increment - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        ASSERT(diminuto_throttle_commit(tp));
        /**/
        ASSERT(!diminuto_throttle_isempty(tp));
        ASSERT(diminuto_throttle_isfull(tp));
        ASSERT(diminuto_throttle_isalarmed(tp));
        ASSERT(!diminuto_throttle_emptied(tp));
        ASSERT(diminuto_throttle_filled(tp));
        ASSERT(diminuto_throttle_alarmed(tp));
        ASSERT(!diminuto_throttle_cleared(tp));
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /**/
        now += increment + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        ASSERT(diminuto_throttle_commit(tp));
        /**/
        ASSERT(!diminuto_throttle_isempty(tp));
        ASSERT(diminuto_throttle_isfull(tp));
        ASSERT(diminuto_throttle_isalarmed(tp));
        ASSERT(!diminuto_throttle_emptied(tp));
        ASSERT(!diminuto_throttle_filled(tp));
        ASSERT(!diminuto_throttle_alarmed(tp));
        ASSERT(!diminuto_throttle_cleared(tp));
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /**/
        now += increment + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(diminuto_throttle_commit(tp));
        /**/
        ASSERT(!diminuto_throttle_isempty(tp));
        ASSERT(!diminuto_throttle_isfull(tp));
        ASSERT(diminuto_throttle_isalarmed(tp));
        ASSERT(!diminuto_throttle_emptied(tp));
        ASSERT(!diminuto_throttle_filled(tp));
        ASSERT(!diminuto_throttle_alarmed(tp));
        ASSERT(!diminuto_throttle_cleared(tp));
        /* REQUEST, RE-REQUESTS, COMMIT */
        now += increment - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(diminuto_throttle_commit(tp));
        /**/
        ASSERT(!diminuto_throttle_isempty(tp));
        ASSERT(!diminuto_throttle_isfull(tp));
        ASSERT(diminuto_throttle_isalarmed(tp));
        ASSERT(!diminuto_throttle_emptied(tp));
        ASSERT(!diminuto_throttle_filled(tp));
        ASSERT(!diminuto_throttle_alarmed(tp));
        ASSERT(!diminuto_throttle_cleared(tp));
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /* REQUEST, DELAY, ADMIT */
        now += increment - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 2;
        ASSERT(diminuto_throttle_admit(tp, now));
        /**/
        ASSERT(!diminuto_throttle_isempty(tp));
        ASSERT(!diminuto_throttle_isfull(tp));
        ASSERT(diminuto_throttle_isalarmed(tp));
        ASSERT(!diminuto_throttle_emptied(tp));
        ASSERT(!diminuto_throttle_filled(tp));
        ASSERT(!diminuto_throttle_alarmed(tp));
        ASSERT(!diminuto_throttle_cleared(tp));
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /* UPDATE */
        now += increment + 10;
        ASSERT(diminuto_throttle_update(tp, now) == 0);
        /**/
        ASSERT(diminuto_throttle_isempty(tp));
        ASSERT(!diminuto_throttle_isfull(tp));
        ASSERT(!diminuto_throttle_isalarmed(tp));
        ASSERT(diminuto_throttle_emptied(tp));
        ASSERT(!diminuto_throttle_filled(tp));
        ASSERT(!diminuto_throttle_alarmed(tp));
        ASSERT(diminuto_throttle_cleared(tp));
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /* SUSTAINED */
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        now += increment;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        /**/
        ASSERT(diminuto_throttle_isempty(tp));
        ASSERT(!diminuto_throttle_isfull(tp));
        ASSERT(!diminuto_throttle_isalarmed(tp));
        ASSERT(!diminuto_throttle_emptied(tp));
        ASSERT(!diminuto_throttle_filled(tp));
        ASSERT(!diminuto_throttle_alarmed(tp));
        ASSERT(!diminuto_throttle_cleared(tp));
        ASSERT(diminuto_throttle_getexpected(&throttle) == throttle.expected);
        /**/
        STATUS();
    }

    {
        diminuto_throttle_t throttle;
        diminuto_throttle_t * tp;
        diminuto_ticks_t increment;
        diminuto_ticks_t limit;
        size_t size;
        diminuto_ticks_t now;
        /**/
        TEST();
        /**/
        tp = &throttle;
        increment = 100;
        limit = 10;
        ASSERT(diminuto_throttle_init(tp, increment, limit, 0) == tp);
        /* SUSTAINED */
        now = 0;
        size = 10;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        /* CONSUME LIMIT */
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        /* FILL */
        now += (size * increment) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        ASSERT(diminuto_throttle_commitn(tp, size));
        now += (size * increment) + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        ASSERT(diminuto_throttle_commitn(tp, size));
        now += (size * increment) + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(diminuto_throttle_commitn(tp, size));
        /* REQUEST, RE-REQUESTS, COMMIT */
        now += (size * increment) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(diminuto_throttle_commitn(tp, size));
        /* REQUEST, DELAY, ADMIT */
        now += (size * increment) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 2;
        ASSERT(diminuto_throttle_admitn(tp, now, size));
        /* SUSTAINED AGAIN */
        now += (size * increment) + 10;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, size));
        /**/
        STATUS();
    }

    {
        diminuto_throttle_t throttle;
        diminuto_throttle_t * tp;
        diminuto_ticks_t increment;
        diminuto_ticks_t limit;
        size_t size;
        diminuto_ticks_t now;
        static const size_t BLOCKSIZE = 32768;
        /**/
        TEST();
        /**/
        tp = &throttle;
        increment = 100;
        limit = 10;
        ASSERT(diminuto_throttle_init(tp, increment, limit, 0) == tp);
        /* SUSTAINED */
        now = 0;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        /* CONSUME LIMIT */
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment) - 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        /* FILL */
        now += (size * increment) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        ASSERT(diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment) + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        ASSERT(diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment) + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        /* REQUEST, RE-REQUESTS, COMMIT */
        now += (size * increment) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        /* REQUEST, DELAY, ADMIT */
        now += (size * increment) - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 2;
        ASSERT(diminuto_throttle_admitn(tp, now, (size = blocksize(BLOCKSIZE))));
        /* SUSTAINED AGAIN */
        now += (size * increment) + 10;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        now += (size * increment);
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commitn(tp, (size = blocksize(BLOCKSIZE))));
        /**/
        STATUS();
    }

    {
        static const size_t BANDWIDTH = 1024;
        static const size_t BLOCKSIZE = 512;
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
        diminuto_ticks_t frequency;
        diminuto_ticks_t increment;
        diminuto_ticks_t limit;
        double sustained;
        double rate;
        double peak = 0;
        /**/
        TEST();
        /**/
        frequency = diminuto_frequency();
        /**/
        increment = diminuto_throttle_interarrivaltime(BANDWIDTH * 3, 3, frequency);
        limit = diminuto_throttle_jittertolerance(increment, BLOCKSIZE);
        /**/
        tp = diminuto_throttle_init(&throttle, increment, 0 /* limit */, now);
        ASSERT(tp == &throttle);
        diminuto_throttle_log(tp);
        srand(diminuto_time_clock());
        /**/
        for (iops = 0; iops < OPERATIONS; ++iops) {
            delay = diminuto_throttle_request(tp, now);
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
            delay = diminuto_throttle_request(tp, now);
            ASSERT(delay == 0);
            size = blocksize(BLOCKSIZE);
            ASSERT(size > 0);
            ASSERT(size <= BLOCKSIZE);
            total += size;
            admissable = !diminuto_throttle_commitn(tp, size);
            ASSERT(admissable);
        }
        /**/
        delay = diminuto_throttle_getexpected(tp);
        ASSERT(delay >= 0);
        now += delay;
        duration += delay;
        if (delay > 0) {
            rate = size;
            rate *= frequency;
            rate /= delay;
            if (rate > peak) { peak = rate; }
        }
        diminuto_throttle_update(tp, now);
        /**/
        ASSERT(total > 0);
        ASSERT(duration > frequency);
        diminuto_throttle_log(tp);
        sustained = total;
        sustained *= frequency;
        sustained /= duration;
        DIMINUTO_LOG_DEBUG("operations=%zu total=%llubytes average=%llubytes duration=%lldseconds requested=%zubytes/second sustained=%lfbytes/second peak=%lfbytes/second\n", iops, total, total / iops, duration / diminuto_frequency(), BANDWIDTH, sustained, peak);
        ASSERT(fabs(sustained - BANDWIDTH) <= (BANDWIDTH / 200) /* 0.5% */);
        ADVISE(fabs(peak - BANDWIDTH) <= (BANDWIDTH / 200) /* 0.5% */);
        /**/
        STATUS();
     }

    EXIT();
}

