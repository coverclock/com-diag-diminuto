/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_memory.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdio.h>

int main(void)
{
	size_t pagesize;
	size_t linesize;
	size_t pagesize2;
	size_t linesize2;
	size_t candidate;
	size_t size;
	size_t alignment;
	size_t power;
	size_t allocated;
	size_t previous;
	int method;

	method = -1;
	pagesize = diminuto_memory_pagesize(&method);
	pagesize2 = diminuto_memory_pagesize((int *)0);
    fprintf(stderr, "pagesize=0x%x=%d method=%d\n", pagesize, pagesize, method);
    ASSERT(method >= 0);
    ASSERT(pagesize > 0);
    ASSERT(pagesize == pagesize2);
    ASSERT(diminuto_memory_is_power(pagesize));

    method = -1;
	linesize = diminuto_memory_linesize(&method);
    linesize2 = diminuto_memory_linesize((int *)0);
	fprintf(stderr, "linesize=0x%x=%d method=%d\n", linesize, linesize, method);
    ASSERT(method >= 0);
	ASSERT(linesize > 0);
	ASSERT(linesize == linesize2);
	ASSERT(diminuto_memory_is_power(linesize));


	ASSERT(!diminuto_memory_is_power(0));
	power = diminuto_memory_power(0);
	ASSERT(power == 1);
    ASSERT(diminuto_memory_is_power(power));

	for (candidate = 1; candidate > 0; candidate <<= 1) {
    	ASSERT(diminuto_memory_is_power(candidate));
    	power = diminuto_memory_power(candidate);
       	ASSERT(power == candidate);
       	ASSERT(diminuto_memory_is_power(power));
   }

	for (candidate = 3; candidate < 0x80000000; candidate = (candidate << 1) | 1) {
    	ASSERT(!diminuto_memory_is_power(candidate));
    	power = diminuto_memory_power(candidate);
     	ASSERT(power > candidate);
    	ASSERT(diminuto_memory_is_power(power));
    }

	for (candidate = 0; candidate <= pagesize; ++candidate) {
    	power = diminuto_memory_power(candidate);
     	ASSERT(power >= candidate);
    	ASSERT(diminuto_memory_is_power(power));
    }

	fprintf(stderr, "%8s %8s %8s %8s\n", "SIZE", "ALIGN", "POWER", "ALLOC");
	previous = 0;
	for (size = 1; size <= DIMINUTO_MEMORY_PAGESIZE_BYTES; ++size) {
		for (alignment = 1; alignment <= DIMINUTO_MEMORY_LINESIZE_BYTES; ++alignment) {
			power = diminuto_memory_power(alignment);
			allocated = diminuto_memory_alignment(size, power);
			if (allocated != previous) {
				fprintf(stderr, "%8u %8u %8u %8u\n", size, alignment, power, allocated);
				previous = allocated;
			}
		}
	}
}
