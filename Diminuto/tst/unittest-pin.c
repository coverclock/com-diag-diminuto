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
#include <stdlib.h>

int main(void)
{
	FILE * pin98;
	FILE * pin99;

	EXPECT(system("rm -rf /tmp/class/gpio") >= 0);
	EXPECT(system("mkdir -p /tmp/class/gpio") >= 0);
	EXPECT(system("mkdir -p /tmp/class/gpio/gpio98") >= 0);
	EXPECT(system("mkdir -p /tmp/class/gpio/gpio99") >= 0);
	EXPECT(system("touch /tmp/class/gpio/export") >= 0);
	EXPECT(system("touch /tmp/class/gpio/gpio98/direction") >= 0);
	EXPECT(system("touch /tmp/class/gpio/gpio98/value") >= 0);
	EXPECT(system("touch /tmp/class/gpio/gpio99/direction") >= 0);
	EXPECT(system("touch /tmp/class/gpio/gpio99/value") >= 0);

	EXPECT(diminuto_pin_debug(!0) == 0);

	ASSERT((pin98 = diminuto_pin_input(98)) != (FILE *)0);
	EXPECT(system("test `tail -1 /tmp/class/gpio/export` == 98") == 0);
	EXPECT(system("test `cat /tmp/class/gpio/gpio98/direction` == in") == 0);

	ASSERT((pin99 = diminuto_pin_output(99)) != (FILE *)0);
	EXPECT(system("test `tail -1 /tmp/class/gpio/export` == 99") == 0);
	EXPECT(system("test `cat /tmp/class/gpio/gpio99/direction` == out") == 0);

	EXPECT(system("echo 0 > /tmp/class/gpio/gpio98/value") == 0);
	EXPECT(system("test `cat /tmp/class/gpio/gpio98/value` -eq 0") == 0);
	EXPECT(!diminuto_pin_get(pin98));
	EXPECT(system("echo 1 > /tmp/class/gpio/gpio98/value") == 0);
	EXPECT(system("test `cat /tmp/class/gpio/gpio98/value` -eq 1") == 0);
	EXPECT(diminuto_pin_get(pin98));
	EXPECT(system("echo 0 > /tmp/class/gpio/gpio98/value") == 0);
	EXPECT(system("test `cat /tmp/class/gpio/gpio98/value` -eq 0") == 0);
	EXPECT(!diminuto_pin_get(pin98));

	EXPECT(diminuto_pin_clear(pin99) >= 0);
	EXPECT(system("test `cat /tmp/class/gpio/gpio99/value` -eq 0") == 0);
	EXPECT(diminuto_pin_set(pin99) >= 0);
	EXPECT(system("test `cat /tmp/class/gpio/gpio99/value` -eq 1") == 0);
	EXPECT(diminuto_pin_clear(pin99) >= 0);
	EXPECT(system("test `cat /tmp/class/gpio/gpio99/value` -eq 0") == 0);

	EXPECT(fclose(pin98) != EOF);
	EXPECT(fclose(pin99) != EOF);

	EXPECT(diminuto_pin_debug(0) == !0);

	EXIT();
}
