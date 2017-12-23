/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <stdio.h>
#include "com/diag/diminuto/diminuto_testify.h"
#include "com/diag/diminuto/diminuto_unittest.h"

#define PRINTF(_VALUE_, _OTHERWISE_) printf("%d %d %s\n", (_VALUE_), (_OTHERWISE_), DIMINUTO_TESTIFY(_VALUE_, _OTHERWISE_))

int main(int argc, char ** argv)
{
	PRINTF(0, 0);
	PRINTF(0, !0);
	PRINTF(!0, 0);
	PRINTF(!0, !0);

	ASSERT(diminuto_testify("activated", 0) == !0);
	ASSERT(diminuto_testify("Activated", 0) == !0);
	ASSERT(diminuto_testify("ACTIVATED", 0) == !0);
	ASSERT(diminuto_testify("active", 0) == !0);
	ASSERT(diminuto_testify("Active", 0) == !0);
	ASSERT(diminuto_testify("ACTIVE", 0) == !0);
	ASSERT(diminuto_testify("asserted", 0) == !0);
	ASSERT(diminuto_testify("Asserted", 0) == !0);
	ASSERT(diminuto_testify("ASSERTED", 0) == !0);
	ASSERT(diminuto_testify("closed", 0) == !0);
	ASSERT(diminuto_testify("Closed", 0) == !0);
	ASSERT(diminuto_testify("CLOSED", 0) == !0);
	ASSERT(diminuto_testify("confirmed", 0) == !0);
	ASSERT(diminuto_testify("Confirmed", 0) == !0);
	ASSERT(diminuto_testify("CONFIRMED", 0) == !0);
	ASSERT(diminuto_testify("enabled", 0) == !0);
	ASSERT(diminuto_testify("Enabled", 0) == !0);
	ASSERT(diminuto_testify("ENABLED", 0) == !0);
	ASSERT(diminuto_testify("high", 0) == !0);
	ASSERT(diminuto_testify("High", 0) == !0);
	ASSERT(diminuto_testify("HIGH", 0) == !0);
	ASSERT(diminuto_testify("on", 0) == !0);
	ASSERT(diminuto_testify("On", 0) == !0);
	ASSERT(diminuto_testify("ON", 0) == !0);
	ASSERT(diminuto_testify("t", 0) == !0);
	ASSERT(diminuto_testify("T", 0) == !0);
	ASSERT(diminuto_testify("true", 0) == !0);
	ASSERT(diminuto_testify("True", 0) == !0);
	ASSERT(diminuto_testify("TRUE", 0) == !0);
	ASSERT(diminuto_testify("up", 0) == !0);
	ASSERT(diminuto_testify("Up", 0) == !0);
	ASSERT(diminuto_testify("UP", 0) == !0);
	ASSERT(diminuto_testify("y", 0) == !0);
	ASSERT(diminuto_testify("Y", 0) == !0);
	ASSERT(diminuto_testify("yes", 0) == !0);
	ASSERT(diminuto_testify("Yes", 0) == !0);
	ASSERT(diminuto_testify("YES", 0) == !0);

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
	ASSERT(diminuto_testify("02", 0) == !0);
	ASSERT(diminuto_testify("03", 0) == !0);
	ASSERT(diminuto_testify("04", 0) == !0);
	ASSERT(diminuto_testify("05", 0) == !0);
	ASSERT(diminuto_testify("06", 0) == !0);
	ASSERT(diminuto_testify("07", 0) == !0);
	ASSERT(diminuto_testify("0x1", 0) == !0);
	ASSERT(diminuto_testify("0x2", 0) == !0);
	ASSERT(diminuto_testify("0x3", 0) == !0);
	ASSERT(diminuto_testify("0x4", 0) == !0);
	ASSERT(diminuto_testify("0x5", 0) == !0);
	ASSERT(diminuto_testify("0x6", 0) == !0);
	ASSERT(diminuto_testify("0x7", 0) == !0);
	ASSERT(diminuto_testify("0x8", 0) == !0);
	ASSERT(diminuto_testify("0x9", 0) == !0);
	ASSERT(diminuto_testify("0xa", 0) == !0);
	ASSERT(diminuto_testify("0xb", 0) == !0);
	ASSERT(diminuto_testify("0xc", 0) == !0);
	ASSERT(diminuto_testify("0xd", 0) == !0);
	ASSERT(diminuto_testify("0xe", 0) == !0);
	ASSERT(diminuto_testify("0xf", 0) == !0);
	ASSERT(diminuto_testify("0XA", 0) == !0);
	ASSERT(diminuto_testify("0XB", 0) == !0);
	ASSERT(diminuto_testify("0XC", 0) == !0);
	ASSERT(diminuto_testify("0XD", 0) == !0);
	ASSERT(diminuto_testify("0XE", 0) == !0);
	ASSERT(diminuto_testify("0XF", 0) == !0);
	ASSERT(diminuto_testify("0xA", 0) == !0);
	ASSERT(diminuto_testify("0xB", 0) == !0);
	ASSERT(diminuto_testify("0xC", 0) == !0);
	ASSERT(diminuto_testify("0xD", 0) == !0);
	ASSERT(diminuto_testify("0xE", 0) == !0);
	ASSERT(diminuto_testify("0xF", 0) == !0);

	ASSERT(diminuto_testify("deactivated", !0) == 0);
	ASSERT(diminuto_testify("Deactivated", !0) == 0);
	ASSERT(diminuto_testify("DEACTIVATED", !0) == 0);
	ASSERT(diminuto_testify("deasserted", !0) == 0);
	ASSERT(diminuto_testify("Deasserted", !0) == 0);
	ASSERT(diminuto_testify("DEASSERTED", !0) == 0);
	ASSERT(diminuto_testify("denied", !0) == 0);
	ASSERT(diminuto_testify("Denied", !0) == 0);
	ASSERT(diminuto_testify("DENIED", !0) == 0);
	ASSERT(diminuto_testify("disabled", !0) == 0);
	ASSERT(diminuto_testify("Disabled", !0) == 0);
	ASSERT(diminuto_testify("DISABLED", !0) == 0);
	ASSERT(diminuto_testify("down", !0) == 0);
	ASSERT(diminuto_testify("Down", !0) == 0);
	ASSERT(diminuto_testify("DOWN", !0) == 0);
	ASSERT(diminuto_testify("f", !0) == 0);
	ASSERT(diminuto_testify("F", !0) == 0);
	ASSERT(diminuto_testify("false", !0) == 0);
	ASSERT(diminuto_testify("False", !0) == 0);
	ASSERT(diminuto_testify("FALSE", !0) == 0);
	ASSERT(diminuto_testify("inactive", !0) == 0);
	ASSERT(diminuto_testify("Inactive", !0) == 0);
	ASSERT(diminuto_testify("INACTIVE", !0) == 0);
	ASSERT(diminuto_testify("low", !0) == 0);
	ASSERT(diminuto_testify("Low", !0) == 0);
	ASSERT(diminuto_testify("LOW", !0) == 0);
	ASSERT(diminuto_testify("n", !0) == 0);
	ASSERT(diminuto_testify("N", !0) == 0);
	ASSERT(diminuto_testify("no", !0) == 0);
	ASSERT(diminuto_testify("No", !0) == 0);
	ASSERT(diminuto_testify("NO", !0) == 0);
	ASSERT(diminuto_testify("off", !0) == 0);
	ASSERT(diminuto_testify("Off", !0) == 0);
	ASSERT(diminuto_testify("OFF", !0) == 0);
	ASSERT(diminuto_testify("open", !0) == 0);
	ASSERT(diminuto_testify("Open", !0) == 0);
	ASSERT(diminuto_testify("OPEN", !0) == 0);

	ASSERT(diminuto_testify("0", !0) == 0);
	ASSERT(diminuto_testify("00", !0) == 0);
	ASSERT(diminuto_testify("0x0", !0) == 0);

	ASSERT(diminuto_testify((const char *)0, 0) == 0);
	ASSERT(diminuto_testify("", 0) == 0);

	ASSERT(diminuto_testify((const char *)0, !0) == !0);
	ASSERT(diminuto_testify("", !0) == !0);

	ASSERT(diminuto_testify((const char *)0, -1) == -1);
	ASSERT(diminuto_testify("", -2) == -2);
	ASSERT(diminuto_testify("o", -3) == -3);
	ASSERT(diminuto_testify("O", -4) == -4);
	ASSERT(diminuto_testify("?", -5) == -5);
	ASSERT(diminuto_testify("0?", -6) == -6);
	ASSERT(diminuto_testify("1?", -7) == -7);
	ASSERT(diminuto_testify("0x0?", -8) == -8);
	ASSERT(diminuto_testify("0X0?", -8) == -8);
	ASSERT(diminuto_testify("0x1?", -8) == -8);
	ASSERT(diminuto_testify("0X1?", -8) == -8);
	ASSERT(diminuto_testify("0x?", -9) == -9);
	ASSERT(diminuto_testify("0X?", -8) == -8);

	EXIT();
}
