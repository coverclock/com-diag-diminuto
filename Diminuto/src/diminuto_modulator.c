/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "../src/diminuto_timer.h"
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

int diminuto_modulator_set(diminuto_modulator_t * mp, int duty)
{
	int rc = -1;
	int on = 0;
	int off = 0;
	int index = 0;
	int prime = 0;
	static const int PRIMES[] = { 7, 5, 3, 2 };

    if (duty <= 0) {
        duty = 0;
    } else if (duty >= 100) {
        duty = 100;
    } else {
       /* Do nothing. */
    }

    on = duty;
    off = 100 - duty;

    for (index = 0; index < countof(PRIMES); ++index) {
    	prime = PRIMES[index];
    	while (((on / prime) > 0) && ((on % prime) == 0) && ((off / prime) > 0) && ((off % prime) == 0)) {
    		on /= prime;
    		off /= prime;
    	}
    }

    DIMINUTO_COHERENT_SECTION_BEGIN;

    	if (mp->set) {
    		errno = EAGAIN;
    		diminuto_perror("diminuto_modulator_set");
    	} else {
    		mp->duty = duty;
    		mp->ton = on;
    		mp->toff = off;
    		mp->set = !0;
    		rc = 0;
    	}

	DIMINUTO_COHERENT_SECTION_END;

	return 0;
}

static void diminuto_modulator_notification(union sigval arg)
{
	diminuto_modulator_t * mp = (diminuto_modulator_t *)0;

	mp = (diminuto_modulator_t *)(arg.sival_ptr);

	do {

		if (mp->cycle > 0) {
			mp->cycle -= 1;
            continue;
        }

        if (mp->condition) {
            if (mp->off > 0) {
			    (void)diminuto_pin_clear(mp->fp);
                mp->condition = 0;
                mp->cycle = mp->off;
            } else {
                mp->cycle = mp->on; /* 100% */
            }
        } else {
            if (mp->on > 0) {
			    (void)diminuto_pin_set(mp->fp);
                mp->condition = !0;
                mp->cycle = mp->on;
            } else {
            	mp->cycle = mp->off; /* 0% */
            }
        }

        if (mp->total < 100) {
        	mp->total += 1;
            continue;
        }

		DIMINUTO_COHERENT_SECTION_BEGIN;

			if (mp->set) {
				mp->on = mp->ton;
				mp->off = mp->toff;
				mp->set = 0;
			}

		DIMINUTO_COHERENT_SECTION_END;

        mp->total = 0;

	} while (0);

	return;
}

int diminuto_modulator_init(diminuto_modulator_t * mp, int pin, int duty) {
	int rc = -1;

	do {

		memset(mp, 0, sizeof(*mp));

	    mp->pin = pin;
	    mp->toff = 100;

	    (void)diminuto_pin_unexport_ignore(pin);

	    mp->fp = diminuto_pin_output(pin);
	    if (mp->fp == (FILE *)0) {
	    	break;
	    }

	    diminuto_modulator_set(mp, duty);

	    rc = 0;

	} while (0);

	return rc;
}

int diminuto_modulator_start(diminuto_modulator_t * mp)
{
	struct sigevent event = { 0 };
	diminuto_sticks_t ticks = 0;

	ticks = diminuto_frequency() / diminuto_modulator_frequency();
	ticks = diminuto_timer_generic(&(mp->initialized), &(mp->timer), &event, ticks, !0);

    return (ticks >= 0);
}

int diminuto_modulator_stop(diminuto_modulator_t * mp)
{
	struct sigevent event = { 0 };
	diminuto_sticks_t ticks = 0;

	ticks = diminuto_timer_generic(&(mp->initialized), &(mp->timer), &event, ticks, !0);

    return (ticks >= 0);
}

int diminuto_modulator_fini(diminuto_modulator_t * mp)
{
	int rc = 0;

	mp->fp = diminuto_pin_unused(mp->fp, mp->pin);
	if (mp->fp != (FILE *)0) {
		rc = -1;
	}

	return rc;
}
