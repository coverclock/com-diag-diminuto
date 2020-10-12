/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_modulator.h"
#include "com/diag/diminuto/diminuto_delay.h"
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
    diminuto_modulator_t modulator;

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

    EXPECT(systemf("mkdir -p %s/class/gpio/gpio%u", root, 99) == 0);

    EXPECT(systemf("touch %s/class/gpio/export", root) == 0);
    EXPECT(systemf("touch %s/class/gpio/unexport", root) == 0);

    EXPECT(systemf("touch %s/class/gpio/gpio%u/direction", root, 99) == 0);
    EXPECT(systemf("touch %s/class/gpio/gpio%u/value", root, 99) == 0);
    EXPECT(systemf("touch %s/class/gpio/gpio%u/edge", root, 99) == 0);
    EXPECT(systemf("touch %s/class/gpio/gpio%u/active_low", root, 99) == 0);

    ASSERT(diminuto_modulator_init(&modulator, 99, 50) == &modulator);

        ASSERT(diminuto_modulator_start(&modulator) >= 0);

            diminuto_delay(diminuto_frequency(), 0);

            ASSERT(diminuto_modulator_set(&modulator, 20) >= 0);

            diminuto_delay(diminuto_frequency(), 0);

            ASSERT(diminuto_modulator_set(&modulator, 60) >= 0);

            diminuto_delay(diminuto_frequency(), 0);

        ASSERT(diminuto_modulator_stop(&modulator) >= 0);

    ASSERT(diminuto_modulator_fini(&modulator) == (diminuto_modulator_t *)0);

    EXPECT(systemf("rm -rf %s", root) == 0);

    prior = diminuto_pin_debug((const char *)0);
    ASSERT(prior != (const char *)0);
    EXPECT(prior == root);

    EXIT();
}

