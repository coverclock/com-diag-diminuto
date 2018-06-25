/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

extern int diminuto_alarm_debug;

static int done = 0;
static int timeouts = 0;
static int wokes = 0;

static void * body(void * arg)
{
	void * xc = 0;
	int rc = 0;
	int ready = 0;

	while (!0) {
		if (!ready) {
			rc = diminuto_alarm_timedwait(diminuto_frequency());
			ASSERT((rc == 0) || (errno == ETIMEDOUT));
			if (rc < 0) {
				COMMENT("thread: timeout.");
				timeouts += 1;
				continue;
			} else if (done) {
				COMMENT("thread: unexpected.");
				xc = (void *)1;
				break;
			} else {
				COMMENT("thread: awoke.");
				wokes += 1;
				ready = !0;
			}
		} else {
			rc = diminuto_alarm_wait();
			ASSERT(rc == 0);
			if (done) {
				COMMENT("thread: done.");
				break;
			} else {
				COMMENT("thread: woke.");
				wokes += 1;
			}
		}
	}

	return xc;
}

int main(int argc, char ** argv)
{
    int rc = 0;
    pthread_t thread; /* Deliberately opaque. */
    void * final = (void *)0;

    SETLOGMASK();

    TEST();

    diminuto_alarm_debug = !0;

    ASSERT(diminuto_alarm_install(!0) == 0);
    ASSERT(!diminuto_alarm_check());

    ASSERT(pthread_create(&thread, (pthread_attr_t *)0, &body, (void *)0) == 0);

    COMMENT("delaying 5 seconds.");

    diminuto_delay(diminuto_frequency() * 5, 0);

    ASSERT(diminuto_timer_periodic(diminuto_frequency()) == 0);

    COMMENT("delaying 10 seconds.");

    diminuto_delay(diminuto_frequency() * 10, 0);

    ASSERT(diminuto_timer_periodic(0) >= 0);
    done = !0;
    ASSERT(diminuto_alarm_broadcast() >= 0);

    COMMENT("delaying 2 seconds.");

    diminuto_delay(diminuto_frequency() * 2, 0);

    ASSERT(pthread_join(thread, &final) == 0);
    ASSERT(final == (void *)0);

	DIMINUTO_COHERENT_SECTION_BEGIN;
		ASSERT(timeouts == 5);
		ASSERT(wokes == 10);
	DIMINUTO_COHERENT_SECTION_END;

    ASSERT((rc = diminuto_alarm_check()) > 0);
    EXPECT(rc == 10);
    ASSERT(!diminuto_alarm_check());

    EXIT();
}
