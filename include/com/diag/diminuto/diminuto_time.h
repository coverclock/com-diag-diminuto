/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIME_
#define _H_COM_DIAG_DIMINUTO_TIME_

/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Return the system clock time in microseconds since the Epoch.
 * @return the number of microseconds elapsed since the Epoch or ~0ULL with
 * errno set if an error occurred.
 */
extern diminuto_usec_t diminuto_time_clock(void);

/**
 * Return the elapsed time in microseconds of a monotonically increasing
 * clock. This value is immune to time adjustments in the system clock and
 * hence is usable to measure duration and elapsed time.
 * @return the number of microseconds elapsed since an arbitrary epoch or
 * ~0ULL with errno set if an error occurred.
 */
extern diminuto_usec_t diminuto_time_elapsed(void);

/**
 * Return the CPU time in microseconds for the calling process.
 * @return the number of microseconds elapsed since an arbitrary epoch or
 * ~0ULL with errno set if an error occurred.
 */
extern diminuto_usec_t diminuto_time_process(void);

/**
 * Return the CPU time in microseconds for the calling thread.
 * @return the number of microseconds elapsed since an arbitrary epoch or
 * ~0ULL with errno set if an error occurred.
 */
extern diminuto_usec_t diminuto_time_thread(void);

#endif
