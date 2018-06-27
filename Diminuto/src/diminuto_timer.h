/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIMER_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_TIMER_PRIVATE_

/**
 * @file
 *
 * Copyright 2017-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * This describes the timer private API exposed for unit testing or other
 * internal uses.
 */

/**
 * Create a POSIX real-time one-shot or periodic timer. Many timers can be
 * created and managed independently. If ticks is zero, the presumably existing
 * timer is cancelled and deleted.
 * @param initializedp points to an initialized flag that is initially false.
 * @param timeridp points to a POSIX timer identifier, initially uninitialized.
 * @param eventp points to a signal event structure that the caller filled in.
 * @param ticks is the number of ticks in the timer's period.
 * @param periodic is true if the timer is periodic, false if a one-shot.
 * @return the number of ticks left in the timer, or <0 if an error occurred.
 */
diminuto_sticks_t diminuto_timer_generic(int * initializedp, timer_t * timeridp, struct sigevent * eventp, diminuto_ticks_t ticks, int periodic);

/**
 * Return the opaque timer_t identifier returned by the timer_create()
 * system call for purposes of unit testing. If the static global
 * Diminuto timer is not initialized or has been deleted, the value
 * (void *)-1 will be returned.
 * @return the timer identifier or -1.
 */
extern timer_t diminuto_timer_singleton_get(void);

/**
 * The singleton getter returns this value is the singleton does not exist.
 */
static const timer_t DIMINUTO_TIMER_UNINITIALIZED = (timer_t)-1;

#endif
