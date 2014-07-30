/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_endianess.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>

int main(void)
{
    int littleendian;
    SETLOGMASK();
	littleendian = diminuto_littleendian();
	DIMINUTO_LOG_INFORMATION("%s %s", DIMINUTO_LOG_HERE, littleendian ? "little-endian" : "big-endian");
	if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
		DIMINUTO_LOG_INFORMATION("%s %s", DIMINUTO_LOG_HERE, "little-endian");
	}
	if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) {
		DIMINUTO_LOG_INFORMATION("%s %s", DIMINUTO_LOG_HERE, "big-endian");
	}
#if defined(__BYTE_ORDER__)
#if defined(__ORDER_LITTLE_ENDIAN__)
#if defined(__ORDER_BIG_ENDIAN__)
	ASSERT(((littleendian) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))||((!littleendian) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)));
#endif
#endif
#endif
    EXIT();
}
