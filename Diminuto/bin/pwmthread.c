/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * USAGE
 *
 * EXAMPLES
 *
 * ABSTRACT
 *
 */

#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_coherentsection.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_terminator.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_types.h"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

static const diminuto_ticks_t HERTZ = 10000; /* 100us */

static const char * program = (const char *)0;

typedef struct Modulator {
	int done;
	pthread_t thread;
	pthread_mutex_t mutex;
	int pin;
	FILE * fp;
	int duty;
	int on;
	int off;
	int state;
} modulator_t;

static void * modulator(void * ap)
{
	intptr_t xc = 0;
	modulator_t * mp = (modulator_t *)0;
	struct sched_param param = { 0 };
	FILE * fp = (FILE *)0;
	int state = 0;
	int on = 0;
	int off = 0;
	int rc = 0;
	int cycle = 0;
    int done = 0;

	mp = (modulator_t *)ap;

	if (geteuid() == 0) {
		param.sched_priority = sched_get_priority_max(SCHED_FIFO);
		rc = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
		if (rc != 0) {
			xc = rc;
		}
	}

	DIMINUTO_CRITICAL_SECTION_BEGIN(&mp->mutex);
		fp = mp->fp;
		state = mp->state;
		on = mp->on;
		off = mp->off;
	DIMINUTO_CRITICAL_SECTION_END;

	while (!done) {
		rc = diminuto_alarm_wait();
		if (rc < 0) {
			xc = errno;
			break;
		}
		if (cycle > 0) {
			cycle -= 1;
		} else if (state && (off > 0)) {
			state = 0;
			rc = diminuto_pin_clear(fp);
			if (rc < 0) {
				xc = errno;
				break;
			}
			DIMINUTO_CRITICAL_SECTION_BEGIN(&mp->mutex);
				off = mp->off;
                done = mp->done;
			DIMINUTO_CRITICAL_SECTION_END;
			cycle = off - 1;
		} else if (!state && (on > 0)) {
			state = !0;
			rc = diminuto_pin_set(fp);
			if (rc < 0) {
				xc = errno;
				break;
			}
			DIMINUTO_CRITICAL_SECTION_BEGIN(&mp->mutex);
				on = mp->on;
                done = mp->done;
			DIMINUTO_CRITICAL_SECTION_END;
			cycle = on - 1;
		} else {
			/* Do nothing. */
		}
	}

	return (void *)xc;
}

static int modulate(modulator_t * mp, int pin, int duty) {
	int xc = -1;
	int rc = -1;
	static const int PRIMES[] = { 7, 5, 3, 2 };
	int index = 0;
	int prime = 0;

	do {

		rc = pthread_mutex_init(&(mp->mutex), (pthread_mutexattr_t *)0);
	    if (rc != 0) {
	    	errno = rc;
	    	diminuto_perror("modulation: pthread_mutex_init");
	    	break;
	    }

	    mp->pin = pin;

	    (void)diminuto_pin_unexport_ignore(mp->pin);

	    mp->fp = diminuto_pin_output(mp->pin);
	    if (mp->fp == (FILE *)0) {
	    	break;
	    }

	    mp->done = 0;

	    if (duty <= 0) {
	        mp->duty = 0;
	    } else if (duty >= 100) {
	        mp->duty = 100;
	    } else {
	       mp->duty = duty;
	    }

	    mp->on = mp->duty;
	    mp->off = 100 - mp->duty;

	    for (index = 0; index < countof(PRIMES); ++index) {
	    	prime = PRIMES[index];
	    	while (((mp->on / prime) > 0) && ((mp->on % prime) == 0) && ((mp->off / prime) > 0) && ((mp->off % prime) == 0)) {
	    		mp->on /= prime;
	    		mp->off /= prime;
	    	}
	    }

	    if (mp->on == 0) {
	    	rc = diminuto_pin_clear(mp->fp);
	    	mp->state = 0;
	    } else if (mp->off == 0) {
	    	rc = diminuto_pin_set(mp->fp);
	    	mp->state = !0;
	    } else {
	    	rc = diminuto_pin_clear(mp->fp);
	    	mp->state = 0;
	    }

	    rc = pthread_create(&(mp->thread), (pthread_attr_t *)0, &modulator, (void*)mp);
	    if (rc != 0) {
	    	errno = rc;
	    	diminuto_perror("modulation: pthread_create");
	    	break;
	    }

	    rc = 0;

	} while (0);

	return rc;
}

static int demodulate(modulator_t * mp)
{
	int xc = 0;
	int rc = -1;
	void * final = (void *)-1;

	do {

    	if (!mp->done) {
            xc = -1;
			break;
		}

		rc = pthread_join(mp->thread, &final);
		if (rc != 0) {
			errno = rc;
			diminuto_perror("demodulation: pthread_join");
			xc = -1;
		}

		if (final != (void *)0) {
			errno = (intptr_t)final;
			diminuto_perror("demodulation: thread");
			xc = -1;
		}

		mp->fp = diminuto_pin_unused(mp->fp, mp->pin);
		if (mp->fp != (FILE *)0) {
			xc = -1;
		}
		mp->pin = -1;

		rc = pthread_mutex_destroy(&(mp->mutex));
		if (rc != 0) {
			errno = rc;
			diminuto_perror("demodulation: pthread_mutex_destroy");
			xc = -1;
		}

	} while (0);

	return xc;
}

int main(int argc, char * argv[])
{
    int xc = 0;
    int pin = -1;
    int duty = 0;
    int rc = 0;
    modulator_t modulation = { 0 };
    diminuto_sticks_t frequency = 0;
    diminuto_sticks_t ticks = 0;

    /*
     * Process arguments from the command line.
     */

    assert(argc == 3);
  
    program = argv[0]; 
    assert(program != (const char *)0);

    pin = atoi(argv[1]);
    assert(pin >= 0);

    duty = atoi(argv[2]);
    assert((0 <= duty) && (duty <= 100));

    /*
     * Install signal handlers.
     */

    rc = diminuto_alarm_install(!0);
    assert(rc >= 0);

    rc = diminuto_terminator_install(!0);
    assert(rc >= 0);

    rc = diminuto_interrupter_install(!0);
    assert(rc >= 0);

    /*
     * Compute the on and off durations from the duty cycle.
     */

    rc = modulate(&modulation, pin, duty);
    printf("%s: rc=%d pin=%d fp=%p done=%d duty=%d on=%d off=%d state=%d\n", program, rc, modulation.pin, modulation.fp, modulation.done, modulation.duty, modulation.on, modulation.off, modulation.state);
    assert(rc == 0);
    assert(modulation.pin == pin);
    assert(modulation.fp != (FILE *)0);
    assert(!modulation.done);
    assert(modulation.duty == duty);
    assert((100 % (modulation.on + modulation.off)) == 0);

    /*
     * Compute the period in ticks based on the frequency in Hertz.
     */

    frequency = diminuto_frequency();
    assert(frequency > 0);

    ticks = frequency / HERTZ;
    assert(ticks > 0);

    /*
     * Set up the work loop.
     */

    ticks = diminuto_timer_periodic(ticks);
    assert(ticks >= 0);

    /*
     * Enter the work loop.
     */

    while (!0) {

        /*
         * Check for a signal.
         */

        (void)pause();

        if (diminuto_terminator_check()) {
            break;
        } else if (diminuto_interrupter_check()) {
            break;
        } else {
            continue;
        }

    }

    /*
     * Tear down the work loop.
     */

    ticks = diminuto_timer_periodic(0);
    assert(ticks >= 0);

    DIMINUTO_CRITICAL_SECTION_BEGIN(&modulation.mutex);
        modulation.done = !0;
    DIMINUTO_CRITICAL_SECTION_END;

    diminuto_alarm_broadcast();

    rc = demodulate(&modulation);
    assert(rc == 0);

    /*
     * Exit.
     */

    return xc;
}
