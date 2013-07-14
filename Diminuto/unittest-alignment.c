/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_well.h"
#include <stdio.h>

int main(void)
{
	size_t size;
	size_t alignment;
	size_t alloc;

	printf("%8s %8s %8s\n", "SIZE", "ALIGN", "ALLOC");
	for (size = 1; size <= 65; ++size) {
		for (alignment = 1; alignment <= 64; alignment *= 2) {
			alloc = diminuto_well_alignment(size, alignment);
			printf("%8u %8u %8u\n", size, alignment, alloc);
		}
	}
}
