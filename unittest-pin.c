/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_pin.h"

int main(void)
{
	FILE * fp;
	FILE * pin98;
	FILE * pin99;

	EXPECT(diminuto_pin_debug(!0) == 0);

	ASSERT((pin98 = diminuto_pin_input(98)) != (FILE *)0);
	ASSERT((pin99 = diminuto_pin_output(99)) != (FILE *)0);

	EXPECT(diminuto_pin_get(pin98) >= 0);
	EXPECT(diminuto_pin_set(pin99, 0) >= 0);
	EXPECT(diminuto_pin_set(pin99, !0) >= 0);

	EXPECT(fclose(pin98) != EOF);
	EXPECT(fclose(pin99) != EOF);

	EXIT();
}
