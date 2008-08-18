/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_DIMINUTO_DELAY_
#define _H_DIMINUTO_DELAY_

/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the GNU GPL V2<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdint.h>

/**
 * Delay the calling process for the specified number of milliseconds.
 * If interruptable, return if interrupted with errno set to EINTR,
 * otherwise continue delaying. Return with errno set to something other
 * then EINTR of an error occurs.
 * @param milliseconds is the desired delay interval in milliseconds.
 * @param interruptable is true if interruptable, false otherwise.
 * @return the number of milliseconds remaining in the delay interval.
 */
extern uint32_t diminuto_delay(uint32_t milliseconds, int interruptable);

#endif
