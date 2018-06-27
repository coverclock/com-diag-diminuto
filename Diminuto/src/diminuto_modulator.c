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
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

static void * diminuto_modulator(void * ap)
{
	intptr_t xc = 0;
	diminuto_modulator_t * mp = (diminuto_modulator_t *)0;
	struct sched_param param = { 0 };
	FILE * fp = (FILE *)0;
	diminuto_modulator_state_t state = DIMINUTO_MODULATOR_ZERO;
	int on = 0;
	int off = 100;
	int rc = 0;
	int cycle = 0;
    int condition = !0;
    int total = 100;

	mp = (diminuto_modulator_t *)ap;

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
	DIMINUTO_CRITICAL_SECTION_END;

	while (state == DIMINUTO_MODULATOR_RUNNING) {

		rc = -1; // diminuto_alarm_wait();
		if (rc < 0) {
		    xc = errno;
			break;
		}

		if (cycle > 0) {
			cycle -= 1;
            continue;
        }

        if (condition) {
            if (off > 0) {
			    rc = diminuto_pin_clear(fp);
                condition = 0;
                cycle = off; 
            } else {
                cycle = on; /* 100% */
            }
        } else {
            if (on > 0) {
			    rc = diminuto_pin_set(fp);
                condition = !0;
                cycle = on;
            } else {
                cycle = off; /* 0% */
            }
        }
        if (rc < 0) {
		    xc = errno;
            break;
        }

        if (total < 100) {
            total += 1;
            continue;
        }

		DIMINUTO_CRITICAL_SECTION_BEGIN(&mp->mutex);
			on = mp->on;
			off = mp->off;
            state = mp->state;
		DIMINUTO_CRITICAL_SECTION_END;

        total = 0;

	}

	DIMINUTO_CRITICAL_SECTION_BEGIN(&mp->mutex);
		mp->state = DIMINUTO_MODULATOR_EXIT;
	DIMINUTO_CRITICAL_SECTION_END;

	return (void *)xc;
}

void diminuto_modulator_set(diminuto_modulator_t * mp, int duty)
{
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

	DIMINUTO_CRITICAL_SECTION_BEGIN(&(mp->mutex));

		mp->duty = duty;
		mp->on = on;
		mp->off = off;

    DIMINUTO_CRITICAL_SECTION_END;
}

int diminuto_modulator_init(diminuto_modulator_t * mp, int pin, int duty) {
	int xc = -1;
	int rc = -1;

	do {

		rc = pthread_mutex_init(&(mp->mutex), (pthread_mutexattr_t *)0);
	    if (rc != 0) {
	    	errno = rc;
	    	diminuto_perror("diminuto_modulator_init: pthread_mutex_init");
	    	break;
	    }

	    mp->pin = pin;

	    (void)diminuto_pin_unexport_ignore(mp->pin);

	    mp->fp = diminuto_pin_output(mp->pin);
	    if (mp->fp == (FILE *)0) {
	    	break;
	    }

	    diminuto_modulator_set(mp, duty);

	    mp->state = DIMINUTO_MODULATOR_READY;

	    rc = 0;

	} while (0);

	return rc;
}

int diminuto_modulator_start(diminuto_modulator_t * mp)
{
	int rc = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&(mp->mutex));

    	if (mp->state == DIMINUTO_MODULATOR_READY) {

    		rc = pthread_create(&(mp->thread), (pthread_attr_t *)0, &diminuto_modulator, (void*)mp);
    		if (rc != 0) {
    			errno = rc;
    			diminuto_perror("diminuto_modulator_start: pthread_create");
                rc = -1;
    		} else {
    			mp->state = DIMINUTO_MODULATOR_RUNNING;
    			rc = 0;
    		}

    	} else {

    		errno = EINVAL;
    		diminuto_perror("diminuto_modulator_start: state");

    	}

    DIMINUTO_CRITICAL_SECTION_END;

    return rc;
}

int diminuto_modulator_stop(diminuto_modulator_t * mp)
{
	int rc = -1;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&(mp->mutex));

    	if (mp->state == DIMINUTO_MODULATOR_RUNNING) {
    		mp->state = DIMINUTO_MODULATOR_DONE;
    		rc = 0;
    	} else {
    		errno = EINVAL;
    		diminuto_perror("diminuto_modulator_stop: state");
    	}

    DIMINUTO_CRITICAL_SECTION_END;

    return rc;
}

int diminuto_modulator_fini(diminuto_modulator_t * mp)
{
	int xc = 0;
	int rc = -1;
	diminuto_modulator_state_t state = DIMINUTO_MODULATOR_ZERO;
	void * final = (void *)-1;

	do {

	    DIMINUTO_CRITICAL_SECTION_BEGIN(&(mp->mutex));

	    	state = mp->state;

	    DIMINUTO_CRITICAL_SECTION_END;

		switch (state) {

		case DIMINUTO_MODULATOR_READY:
			break;

		case DIMINUTO_MODULATOR_DONE:
		case DIMINUTO_MODULATOR_EXIT:
        	rc = pthread_join(mp->thread, &final);
			if (rc != 0) {
				errno = rc;
				diminuto_perror("diminuto_modulator_fini: pthread_join");
				xc = -1;
			} else if (final != (void *)0) {
				errno = (intptr_t)final;
				diminuto_perror("diminuto_modulator_fini: thread");
				xc = -1;
			} else {
				/* Do nothing. */
			}
			break;

		default:
			errno = EINVAL;
			diminuto_perror("diminuto_modulator_fini: state");
			xc = -1;
			break;

		}

		if (xc != 0) {
			break;
		}

        rc = diminuto_pin_clear(mp->fp);
        if (rc < 0) {
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
			diminuto_perror("diminuto_modulator_fini: pthread_mutex_destroy");
			xc = -1;
		}

		mp->state = DIMINUTO_MODULATOR_ZERO;

	} while (0);

	return xc;
}
