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
#include <stdio.h>

int main(int argc, char ** argv)
{
	const char * p;
	uint32_t n;

	--argc;
	++argv;
	while ((argc--) > 0) {
		p = diminuto_number(*(argv++), &n);
		printf("0%lo 0x%lx %lu %ld 0x%x\n", n, n, n, n, *p);
	}

	return 0;
}
