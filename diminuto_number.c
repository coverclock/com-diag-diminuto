/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_number.h"
#include <stdlib.h>
#include <errno.h>

const char * diminuto_number(const char * s, uint32_t * p)
{
	uint32_t n;
	char * e;

	n = strtoul(s, &e, 0);

	if (p != (uint32_t *)0) { *p = n; }
	if (*e != '\0') { errno = EINVAL; }

	return e;
}
