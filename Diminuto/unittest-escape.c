/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_escape.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <string.h>
#include <stdio.h>

int main(void)
{
	{
		const char ONE[] = "\0\a\b\f\n\r\t\v\1\23\123\xa\xBC";
		char two[64];
		char three[64];
		size_t size;

		diminuto_dump(stdout, ONE, sizeof(ONE));
		size = diminuto_escape_expand(two, ONE, sizeof(two), sizeof(ONE), (const char *)0);
		printf("\"%s\"\n", two);
		size = diminuto_escape_collapse(three, two, strlen(two));
		diminuto_dump(stdout, three, size);

	}


    EXIT();
}
