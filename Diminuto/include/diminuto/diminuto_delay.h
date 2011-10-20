/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DELAY_
#define _H_COM_DIAG_DIMINUTO_DELAY_

/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto/diminuto_types.h"

/**
 * Delay the calling process for the specified number of microseconds.
 * If interruptable, return if interrupted with errno set to EINTR,
 * otherwise continue delaying. Return with errno set to something other
 * then EINTR of an error occurs. The actual delay duration will be approximate
 * depending on the granularity of the system clock and latency in the
 * implementation. If a delay of zero microseconds is specified, the behavior
 * of the function is undefined and will depend on what the underlying platform
 * does.
 * @param microseconds is the desired delay interval in microseconds.
 * @param interruptable is true if interruptable, false otherwise.
 * @return the number of microseconds remaining in the delay duration if
 * the function returned prematurely.
 */
extern diminuto_usec_t diminuto_delay(diminuto_usec_t microseconds, int interruptable);

/**
 * Force a context switch without otherwise blocking.
 * @return 0 if successful, <0 with errno set otherwise.
 */
extern int diminuto_yield(void);

#endif
