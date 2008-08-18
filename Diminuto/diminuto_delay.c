/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the GNU GPL V2<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_delay.h"
#include <stdio.h>
#include <errno.h>
/* #define _POSIX_C_SOURCE 199309 */
#include <time.h>

uint32_t diminuto_delay(uint32_t milliseconds, int interruptable)
{
	struct timespec delay;
	struct timespec remaining;

	delay.tv_sec = milliseconds / 1000U;
	delay.tv_nsec = (milliseconds % 1000U) * 1000000U;

	remaining = delay;

	while (nanosleep(&delay, &remaining) != 0) {
		if (errno != EINTR) { break; }
		if (interruptable) { break; }
		delay = remaining;
	}

	return (remaining.tv_sec * 1000U) + (remaining.tv_nsec / 1000000U);
}
