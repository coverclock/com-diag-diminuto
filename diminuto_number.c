/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008-2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_number.h"
#include <stdlib.h>
#include <errno.h>

const char * diminuto_number_unsigned(const char * s, diminuto_unsigned_t * p)
{
	diminuto_unsigned_t n;
	char * e;

	n = strtoull(s, &e, 0);

	if (p != (diminuto_unsigned_t *)0) { *p = n; }
	if (*e != '\0') { errno = EINVAL; }

	return e;
}

const char * diminuto_number_signed(const char * s, diminuto_signed_t * p)
{
	diminuto_signed_t n;
	char * e;

	n = strtoll(s, &e, 0);

	if (p != (diminuto_signed_t *)0) { *p = n; }
	if (*e != '\0') { errno = EINVAL; }

	return e;
}
