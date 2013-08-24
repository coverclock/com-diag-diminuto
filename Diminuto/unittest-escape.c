/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
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
		char one[1 << (sizeof(char) * 8)];
		char two[(sizeof(one) * (sizeof("\\xff") - 1)) + sizeof('\0')];
		char three[sizeof(two)];
		size_t size;

		for (size = 0; size < sizeof(one); ++size) {
			one[size] = size + 1;
		}
		fputs("one\n", stdout);
		diminuto_dump(stdout, one, sizeof(one));
		size = diminuto_escape_expand(two, one, sizeof(two), sizeof(one), "\" ");
		fprintf(stdout, "two \"%s\"\n", two);
		diminuto_dump(stdout, two, size);
		size = diminuto_escape_collapse(three, two, sizeof(three));
		fputs("three\n", stdout);
		diminuto_dump(stdout, three, size);
	}

    EXIT();
}
