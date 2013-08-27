/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Concha.html<BR>
 */

#include "com/diag/diminuto/diminuto_fletcher.h"

void * diminuto_fletcher16_buffer(const void * buffer, size_t length, uint8_t * ap, uint8_t * bp)
{
	const uint8_t * pp;

	for (pp = (const uint8_t *)buffer; length > 0; --length) {
		diminuto_fletcher16_datum(*(pp++), ap, bp);
	}

	return (void *)pp;
}
