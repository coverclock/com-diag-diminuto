/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Concha.html<BR>
 */

#include "diminuto/diminuto_fletcher8.h"

void * diminuto_fletcher8(const void * buffer, size_t length, uint8_t * ap, uint8_t * bp)
{
	uint8_t * pp;

	for (pp = (uint8_t *)buffer; length > 0; --length) {
        *ap += *(pp++);
        *bp += *ap;
	}

	return pp;
}
