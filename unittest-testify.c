/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_testify.h"
#include "com/diag/diminuto/diminuto_unittest.h"

int main(int argc, char ** argv)
{
	ASSERT(diminuto_testify((const char *)0, 0) == 0);
	ASSERT(diminuto_testify("", 0) == 0);

	ASSERT(diminuto_testify((const char *)0, !0) == !0);
	ASSERT(diminuto_testify("", !0) == !0);

	ASSERT(diminuto_testify((const char *)0, -1) == -1);
	ASSERT(diminuto_testify("", -1) == -1);

	ASSERT(diminuto_testify("asserted", 0) == !0);
	ASSERT(diminuto_testify("closed", 0) == !0);
	ASSERT(diminuto_testify("confirmed", 0) == !0);
	ASSERT(diminuto_testify("enabled", 0) == !0);
	ASSERT(diminuto_testify("high", 0) == !0);
	ASSERT(diminuto_testify("t", 0) == !0);
	ASSERT(diminuto_testify("true", 0) == !0);
	ASSERT(diminuto_testify("y", 0) == !0);
	ASSERT(diminuto_testify("yes", 0) == !0);
	ASSERT(diminuto_testify("on", 0) == !0);
	ASSERT(diminuto_testify("Asserted", 0) == !0);
	ASSERT(diminuto_testify("Closed", 0) == !0);
	ASSERT(diminuto_testify("Confirmed", 0) == !0);
	ASSERT(diminuto_testify("Enabled", 0) == !0);
	ASSERT(diminuto_testify("High", 0) == !0);
	ASSERT(diminuto_testify("T", 0) == !0);
	ASSERT(diminuto_testify("True", 0) == !0);
	ASSERT(diminuto_testify("Y", 0) == !0);
	ASSERT(diminuto_testify("Yes", 0) == !0);
	ASSERT(diminuto_testify("On", 0) == !0);
	ASSERT(diminuto_testify("ASSERTED", 0) == !0);
	ASSERT(diminuto_testify("CLOSED", 0) == !0);
	ASSERT(diminuto_testify("CONFIRMED", 0) == !0);
	ASSERT(diminuto_testify("ENABLED", 0) == !0);
	ASSERT(diminuto_testify("HIGH", 0) == !0);
	ASSERT(diminuto_testify("TRUE", 0) == !0);
	ASSERT(diminuto_testify("YES", 0) == !0);
	ASSERT(diminuto_testify("ON", 0) == !0);
	ASSERT(diminuto_testify("1", 0) == !0);
	ASSERT(diminuto_testify("2", 0) == !0);
	ASSERT(diminuto_testify("3", 0) == !0);
	ASSERT(diminuto_testify("4", 0) == !0);
	ASSERT(diminuto_testify("5", 0) == !0);
	ASSERT(diminuto_testify("6", 0) == !0);
	ASSERT(diminuto_testify("7", 0) == !0);
	ASSERT(diminuto_testify("8", 0) == !0);
	ASSERT(diminuto_testify("9", 0) == !0);
	ASSERT(diminuto_testify("01", 0) == !0);
	ASSERT(diminuto_testify("0x1", 0) == !0);

	ASSERT(diminuto_testify("disabled", !0) == 0);
	ASSERT(diminuto_testify("deasserted", !0) == 0);
	ASSERT(diminuto_testify("denied", !0) == 0);
	ASSERT(diminuto_testify("f", !0) == 0);
	ASSERT(diminuto_testify("false", !0) == 0);
	ASSERT(diminuto_testify("n", !0) == 0);
	ASSERT(diminuto_testify("no", !0) == 0);
	ASSERT(diminuto_testify("low", !0) == 0);
	ASSERT(diminuto_testify("off", !0) == 0);
	ASSERT(diminuto_testify("open", !0) == 0);
	ASSERT(diminuto_testify("Disabled", !0) == 0);
	ASSERT(diminuto_testify("Deasserted", !0) == 0);
	ASSERT(diminuto_testify("Denied", !0) == 0);
	ASSERT(diminuto_testify("F", !0) == 0);
	ASSERT(diminuto_testify("False", !0) == 0);
	ASSERT(diminuto_testify("N", !0) == 0);
	ASSERT(diminuto_testify("No", !0) == 0);
	ASSERT(diminuto_testify("Low", !0) == 0);
	ASSERT(diminuto_testify("Off", !0) == 0);
	ASSERT(diminuto_testify("Open", !0) == 0);
	ASSERT(diminuto_testify("DISABLED", !0) == 0);
	ASSERT(diminuto_testify("DEASSERTED", !0) == 0);
	ASSERT(diminuto_testify("DENIED", !0) == 0);
	ASSERT(diminuto_testify("FALSE", !0) == 0);
	ASSERT(diminuto_testify("NO", !0) == 0);
	ASSERT(diminuto_testify("LOW", !0) == 0);
	ASSERT(diminuto_testify("OFF", !0) == 0);
	ASSERT(diminuto_testify("OPEN", !0) == 0);
	ASSERT(diminuto_testify("0", !0) == 0);
	ASSERT(diminuto_testify("00", !0) == 0);
	ASSERT(diminuto_testify("0x0", !0) == 0);

	ASSERT(diminuto_testify("o", -1) == -1);
	ASSERT(diminuto_testify("O", -1) == -1);
	ASSERT(diminuto_testify("?", -1) == -1);

    EXIT();
}
