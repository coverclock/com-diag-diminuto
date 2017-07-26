/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIMER_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_TIMER_PRIVATE_

/**
 * @file
 *
 * Copyright 2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This describes the timer private API exposed for unit testing.
 */

/**
 * Return the opaque timer_t identifier returned by the timer_create()
 * system call for purposes of unit testing. If the static global
 * Diminuto timer is not initialized or has been deleted, the value
 * (void *)-1 will be returned.
 * @return the timer identifier or -1.
 */
extern void * diminuto_ptimer_get(void);

#endif
