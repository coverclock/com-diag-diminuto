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

static void dump(diminuto_throttle_t * tp)
{
     fprintf(stderr, "diminuto_throttle_t@%p[%zu]: { (now-then)=%lld increment=%lld limit=%lld expected=%lld actual=%lld alarmed=%d alarming=%d }\n", tp, sizeof(*tp), tp->now - tp->then, tp->increment, tp->limit, tp->expected, tp->actual, tp->alarmed, tp->alarming);
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
        /**/
        throttle.now = ~throttle.now;
        throttle.then = ~throttle.then;
        throttle.expected = ~throttle.expected;
        throttle.actual = ~throttle.actual;
        throttle.alarmed = ~throttle.alarmed;
        throttle.alarming = ~throttle.alarming;
        ASSERT(throttle.increment == II);
        ASSERT(throttle.limit == LL);
        ASSERT(throttle.now != TT);
        ASSERT(throttle.then != (TT - II));
        ASSERT(throttle.expected != II);
        ASSERT(throttle.actual != 0);
        ASSERT(throttle.alarmed != 0);
        ASSERT(throttle.alarming != 0);
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
        /**/
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
        /**/
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
        /**/
        now += ii - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        ASSERT(diminuto_throttle_commit(tp));
        now += ii + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        ASSERT(diminuto_throttle_commit(tp));
        now += ii + 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        /**/
        now += ii - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 1);
        now += 1;
        ASSERT(diminuto_throttle_request(tp, now) == 0);
        ASSERT(!diminuto_throttle_commit(tp));
        /**/
        now += ii - 2;
        ASSERT(diminuto_throttle_request(tp, now) == 2);
        now += 2;
        ASSERT(!diminuto_throttle_admit(tp, now));
    }

    EXIT();
}

