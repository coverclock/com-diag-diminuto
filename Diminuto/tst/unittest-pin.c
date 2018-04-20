/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>

static const char TMP[] = "/tmp";


static int systemf(const char * format, ...)
{
	int rc;
	va_list ap;
	char buffer[PATH_MAX];

	DIMINUTO_LOG_DEBUG("%s\"%s\"", DIMINUTO_LOG_HERE, format);

	va_start(ap, format);
	vsnprintf(buffer, sizeof(buffer), format, ap);
	va_end(ap);

	DIMINUTO_LOG_DEBUG("%s\"%s\"", DIMINUTO_LOG_HERE, buffer);

	rc = system(buffer);

	return rc;
}

int main(int argc, char ** argv)
{
	const char * root;
	const char * prior;
	char buffer[PATH_MAX];
	FILE * pin98;
	FILE * pin99;

	SETLOGMASK();

	DIMINUTO_LOG_DEBUG("%s%u", DIMINUTO_LOG_HERE, sizeof(buffer));
	root = ((root = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : root + 1;
	DIMINUTO_LOG_DEBUG("%s\"%s\"", DIMINUTO_LOG_HERE, root);
	snprintf(buffer, sizeof(buffer), "%s/%s-XXXXXX", TMP, root);
	DIMINUTO_LOG_DEBUG("%s\"%s\"", DIMINUTO_LOG_HERE, buffer);
	root = mkdtemp(buffer);
	ASSERT((root != NULL) && (*root != '\0'));
	DIMINUTO_LOG_DEBUG("%s\"%s\"", DIMINUTO_LOG_HERE, root);
	prior = diminuto_pin_debug(root);
	ASSERT((prior != (const char *)0) && (*prior != '\0'));
	DIMINUTO_LOG_DEBUG("%s\"%s\"", DIMINUTO_LOG_HERE, root);

	EXPECT(systemf("mkdir -p %s/class/gpio/gpio%u", root, 98) == 0);
	EXPECT(systemf("mkdir -p %s/class/gpio/gpio%u", root, 99) == 0);

	EXPECT(systemf("touch %s/class/gpio/export", root) == 0);
	EXPECT(systemf("touch %s/class/gpio/unexport", root) == 0);

	EXPECT(systemf("touch %s/class/gpio/gpio%u/direction", root, 98) == 0);
	EXPECT(systemf("touch %s/class/gpio/gpio%u/value", root, 98) == 0);
	EXPECT(systemf("touch %s/class/gpio/gpio%u/edge", root, 98) == 0);
	EXPECT(systemf("touch %s/class/gpio/gpio%u/active_low", root, 98) == 0);

	EXPECT(systemf("touch %s/class/gpio/gpio%u/direction", root, 99) == 0);
	EXPECT(systemf("touch %s/class/gpio/gpio%u/value", root, 99) == 0);
	EXPECT(systemf("touch %s/class/gpio/gpio%u/edge", root, 99) == 0);
	EXPECT(systemf("touch %s/class/gpio/gpio%u/active_low", root, 99) == 0);

	ASSERT((pin98 = diminuto_pin_input(98)) != (FILE *)0);
	EXPECT(systemf("cat %s/class/gpio/export", root, 98) == 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/direction", root, 98) == 0);
	EXPECT(systemf("test `tail -1 %s/class/gpio/export` -eq %u", root, 98) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/direction` = %s", root, 98, "in") == 0);

	ASSERT((pin99 = diminuto_pin_output(99)) != (FILE *)0);
	EXPECT(systemf("cat %s/class/gpio/export", root, 99) == 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/direction", root, 99) == 0);
	EXPECT(systemf("test `tail -1 %s/class/gpio/export` -eq %u", root, 99) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/direction` = %s", root, 99, "out") == 0);

	ASSERT(diminuto_pin_active(99, 0) >= 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/active_low", root, 99) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/active_low` -eq %s", root, 99, "1") == 0);
	ASSERT(diminuto_pin_active(99, !0) >= 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/active_low", root, 99) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/active_low` -eq %s", root, 99, "0") == 0);

	ASSERT(diminuto_pin_edge(99, DIMINUTO_PIN_EDGE_RISING) >= 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/edge", root, 99) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/edge` = %s", root, 99, "rising") == 0);
	ASSERT(diminuto_pin_edge(99, DIMINUTO_PIN_EDGE_FALLING) >= 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/edge", root, 99) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/edge` = %s", root, 99, "falling") == 0);
	ASSERT(diminuto_pin_edge(99, DIMINUTO_PIN_EDGE_BOTH) >= 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/edge", root, 99) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/edge` = %s", root, 99, "both") == 0);
	ASSERT(diminuto_pin_edge(99, DIMINUTO_PIN_EDGE_NONE) >= 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/edge", root, 99) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/edge` = %s", root, 99, "none") == 0);

	EXPECT(systemf("echo 0 > %s/class/gpio/gpio%u/value", root, 98) == 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/value", root, 98) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/value` -eq %u", root, 98, 0) == 0);
	EXPECT(!diminuto_pin_get(pin98));
	EXPECT(systemf("echo 1 > %s/class/gpio/gpio%u/value", root, 98) == 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/value", root, 98) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/value` -eq %u", root, 98, 1) == 0);
	EXPECT(diminuto_pin_get(pin98));
	EXPECT(systemf("echo 0 > %s/class/gpio/gpio%u/value", root, 98) == 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/value", root, 98) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/value` -eq %u", root, 98, 0) == 0);
	EXPECT(!diminuto_pin_get(pin98));

	EXPECT(diminuto_pin_clear(pin99) >= 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/value", root, 99) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/value` -eq %u", root, 99, 0) == 0);
	EXPECT(diminuto_pin_set(pin99) >= 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/value", root, 99) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/value` -eq %u", root, 99, 1) == 0);
	EXPECT(diminuto_pin_clear(pin99) >= 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/value", root, 99) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/value` -eq %u", root, 99, 0) == 0);

	ASSERT(diminuto_pin_initialize(98, 0) >= 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/direction", root, 98) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/direction` = %s", root, 98, "low") == 0);
	ASSERT(diminuto_pin_initialize(98, !0) >= 0);
	EXPECT(systemf("cat %s/class/gpio/gpio%u/direction", root, 98) == 0);
	EXPECT(systemf("test `cat %s/class/gpio/gpio%u/direction` = %s", root, 98, "high") == 0);

	EXPECT(diminuto_pin_unused(pin98, 98) == (FILE *)0);
	EXPECT(systemf("cat %s/class/gpio/unexport", root, 98) == 0);
	EXPECT(systemf("test `tail -1 %s/class/gpio/unexport` -eq %u", root, 98) == 0);
	EXPECT(diminuto_pin_unused(pin99, 99) == (FILE *)0);
	EXPECT(systemf("cat %s/class/gpio/unexport", root, 99) == 0);
	EXPECT(systemf("test `tail -1 %s/class/gpio/unexport` -eq %u", root, 99) == 0);
	EXPECT(diminuto_pin_unused((FILE *)0, 100) == (FILE *)0);

	EXPECT(systemf("rm -rf %s", root) == 0);

	prior = diminuto_pin_debug((const char *)0);
	ASSERT(prior != (const char *)0);
	EXPECT(prior == root);

	EXIT();
}
