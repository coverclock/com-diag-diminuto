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
#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdio.h>

int main(void)
{
	size_t pagesize;
	size_t linesize;
	size_t candidate;
	size_t size;
	size_t alignment;
	size_t power;
	size_t allocated;
	size_t previous;

	pagesize = diminuto_well_pagesize();
    fprintf(stderr, "pagesize=0x%x=%d\n", pagesize, pagesize);
    ASSERT(diminuto_well_is_power(pagesize));

	linesize = diminuto_well_linesize();
	fprintf(stderr, "linesize=0x%x=%d\n", linesize, linesize);
	ASSERT(diminuto_well_is_power(linesize));

	fprintf(stderr, "%8s %8s %8s %8s\n", "SIZE", "ALIGN", "POWER", "ALLOC");
	previous = 0;
	for (size = 1; size <= DIMINUTO_WELL_PAGESIZE; ++size) {
		for (alignment = 1; alignment <= DIMINUTO_WELL_LINESIZE; ++alignment) {
			power = diminuto_well_power(alignment);
			allocated = diminuto_well_alignment(size, power);
			if (allocated != previous) {
				fprintf(stderr, "%8u %8u %8u %8u\n", size, alignment, power, allocated);
				previous = allocated;
			}
		}
	}

	power = diminuto_well_power(0);
	ASSERT(power == 1);
    ASSERT(diminuto_well_is_power(power));

	for (candidate = 1; candidate > 0; candidate <<= 1) {
    	ASSERT(diminuto_well_is_power(candidate));
    	power = diminuto_well_power(candidate);
       	ASSERT(power == candidate);
       	ASSERT(diminuto_well_is_power(power));
   }

	for (candidate = 3; candidate < 0x80000000; candidate = (candidate << 1) | 1) {
    	ASSERT(!diminuto_well_is_power(candidate));
    	power = diminuto_well_power(candidate);
     	ASSERT(power > candidate);
    	ASSERT(diminuto_well_is_power(power));
    }

	for (candidate = 0; candidate <= pagesize; ++candidate) {
    	power = diminuto_well_power(candidate);
     	ASSERT(power >= candidate);
    	ASSERT(diminuto_well_is_power(power));
    }
}
