/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

int diminuto_alarm_debug = 0; /* Not part of the public API. */

static int signaled = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

int diminuto_alarm_timedwait(diminuto_sticks_t timeout)
{
	int rc = 0;
	struct timeval val = { 0 };
	struct timespec spec = { 0 };

	do {

		/*
		 * Whoever decided that the timeout on the condition timed wait should
		 * be an absolute clock time has some 'splaining to do. (I very dimly
		 * recall learning why this must be the case some eons ago.)
		 */

		if (timeout < 0) {
			/* Do nothing. */
		} else if ((rc = gettimeofday(&val, (struct timezone *)0)) < 0) {
			diminuto_perror("diminuto_alarm_timedwait: gettimeofday");
			break;
		} else {
			spec.tv_sec = val.tv_sec;
			spec.tv_nsec = val.tv_usec * 1000;
			spec.tv_sec += diminuto_frequency_ticks2wholeseconds(timeout);
			spec.tv_nsec += diminuto_frequency_ticks2fractionalseconds(timeout, 1000000000);
		}

		if (timeout < 0) {
			/* Do nothing. */
		} else if (!diminuto_alarm_debug) {
			/* Do nothing. */
		} else {
			DIMINUTO_LOG_DEBUG("diminuto_alarm_timedwait: timeout=%lldticks val=<%lds,%ldus> spec=<%lds,%ldns>", timeout, val.tv_sec, val.tv_usec, spec.tv_sec, spec.tv_nsec);
		}

		DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);

			rc = (timeout < 0) ? pthread_cond_wait(&condition, &mutex) : pthread_cond_timedwait(&condition, &mutex, &spec);

		DIMINUTO_CRITICAL_SECTION_END;

		if (rc == 0) {
			/* Do nothing. */
		} else if (timeout < 0) {
			errno = rc;
			diminuto_perror("diminuto_alarm_wait: pthread_cond_wait");
			rc = -1;
		} else if (rc == ETIMEDOUT) {
			errno = rc;
			rc = -1;
		} else {
			errno = rc;
			diminuto_perror("diminuto_alarm_timedwait: pthread_cond_timedwait");
			rc = -1;
		}

	} while (0);

	return rc;
}

int diminuto_alarm_broadcast(void)
{
	int rc = 0;

	rc = pthread_cond_broadcast(&condition);

	if (rc != 0) {
		errno = rc;
		diminuto_perror("diminuto_alarm_broadcast: pthread_cond_broadcast");
		rc = -1;
	}

	return rc;
}

int diminuto_alarm_signal(pid_t pid)
{
    int rc = 0;

    if (kill(pid, SIGALRM) < 0) {
        diminuto_perror("diminuto_alarm_signal: kill");
        rc = -1;
    } else if (diminuto_alarm_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_alarm_signal: SIGALRM");
    } else {
        /* Do nothing. */
    }

    return rc;
}

static void diminuto_alarm_handler(int signum)
{
	int rc = 0;

    if (signum != SIGALRM) {
    	/* Do nothing. */
    } else if (signaled < (~(((int)1) << ((sizeof(signaled) * 8) - 1)))) {
        signaled += 1;
    	rc = pthread_cond_broadcast(&condition);
    	if (rc != 0) {
    		DIMINUTO_LOG_ERROR("diminuto_alarm_handler: pthread_cond_broadcast: %s", strerror(rc));
    	}
    } else {
    	/* Do nothing. */
    }
}

int diminuto_alarm_check(void)
{
    int mysignaled;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
    	DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGALRM);

        	mysignaled = signaled;
        	signaled = 0;

        DIMINUTO_UNINTERRUPTIBLE_SECTION_END;
    DIMINUTO_CRITICAL_SECTION_END;

    if (!mysignaled) {
        /* Do nothing. */
    } else if (diminuto_alarm_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_alarm_check: SIGALRM");
    } else {
        /* Do nothing. */
    }

    return mysignaled;
}

int diminuto_alarm_install(int restart)
{
    int rc = 0;
    struct sigaction alarm = { 0 };

    alarm.sa_handler = diminuto_alarm_handler;
    alarm.sa_flags = restart ? SA_RESTART : 0;

    if (sigaction(SIGALRM, &alarm, (struct sigaction *)0) < 0) {
        diminuto_perror("diminuto_alarm_install: sigaction");
        rc = -1;
    } else if (diminuto_alarm_debug) {
        DIMINUTO_LOG_DEBUG("diminuto_alarm_install: SIGALRM");
    } else {
        /* Do nothing. */
    }

    return rc;
}
