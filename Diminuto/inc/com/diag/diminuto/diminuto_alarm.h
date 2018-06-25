/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ALARM_
#define _H_COM_DIAG_DIMINUTO_ALARM_

/**
 * @file
 *
 * Copyright 2009-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * @def COM_DIAG_DIMINUTO_ALARM_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you chould always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_ALARM_FREQUENCY (1000000000LL)

/**
 * Return the resolution of the Diminuto alarm time units in ticks per second
 * (Hertz).
 * @return the resolution in ticks per second.
 */
static inline diminuto_sticks_t diminuto_alarm_frequency(void) {
    return COM_DIAG_DIMINUTO_ALARM_FREQUENCY;
}

/**
 * Send a process a SIGALRM signal.
 * @param pid is the process identifier, or 0 for process group, or -1 for all
 * processes for whom the sender has permissions to send a signal.
 * @return 0 or <0 if an error occurred.
 */
extern int diminuto_alarm_signal(pid_t pid);

/**
 * Return the number of SIGALRM signals received since the previous call.
 * @return the number of SIGALRM signals received since the previous call.
 */
extern int diminuto_alarm_check(void);

/**
 * Install a SIGALRM signal handler in the caller.
 * @param restart is true if interrupt system calls should be restarted.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_alarm_install(int restart);

/**
 * Block the calling thread until the SIGALRM signal handler broadcasts the
 * alarm, or until the timeout expires.
 * @param timeout is the thread wait timeout in ticks, or <0 for no timeout.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_alarm_timedwait(diminuto_sticks_t timeout);

/**
 * Block the calling thread until the SIGALRM signal handler broadcasts the
 * alarm.
 * @return 0 for success, <0 otherwise.
 */
static inline int diminuto_alarm_wait(void) {
	return diminuto_alarm_timedwait((diminuto_sticks_t)-1);
}

/**
 * Broadcast the alarm to all threads waiting on the condition.
 * @return 0 for success, <0 otherwise.
 */
extern int diminuto_alarm_broadcast(void);

#endif
