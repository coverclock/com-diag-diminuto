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

    DIMINUTO_LOG_DEBUG("%sformat=\"%s\"", DIMINUTO_LOG_HERE, format);

    va_start(ap, format);
    vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    DIMINUTO_LOG_DEBUG("%scommand=\"%s\"", DIMINUTO_LOG_HERE, buffer);

    ASSERT((rc = system(buffer)) >= 0);

    return rc;
}

int main(int argc, char ** argv)
{
    const char * root;
    const char * prior;
    char buffer[PATH_MAX];
    diminuto_modulator_t modulator;

    SETLOGMASK();

    TEST();

    DIMINUTO_LOG_DEBUG("%ssizeof=%u", DIMINUTO_LOG_HERE, sizeof(buffer));
    ASSERT(argv[0] != (char *)0);
    root = strrchr(argv[0], '/');
    root = (root == (char *)0) ? argv[0] : root + 1;
    ASSERT(root != (const char *)0);
    DIMINUTO_LOG_DEBUG("%sroot=\"%s\"", DIMINUTO_LOG_HERE, root);
    memset(&buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s/%s-XXXXXX", TMP, root);
    ASSERT(buffer[sizeof(buffer) - 1] == '\0');
    DIMINUTO_LOG_DEBUG("%sbuffer=\"%s\"", DIMINUTO_LOG_HERE, buffer);
    root = mkdtemp(buffer);
    ASSERT(root != (const char *)0);
    ASSERT(*root != '\0');
    DIMINUTO_LOG_DEBUG("%smkdtemp=\"%s\"", DIMINUTO_LOG_HERE, root);
    prior = diminuto_pin_debug(root);
    ASSERT(prior != (const char *)0);
    ASSERT(*prior != '\0');
    DIMINUTO_LOG_DEBUG("%sprior=\"%s\"", DIMINUTO_LOG_HERE, prior);

    EXPECT(systemf("mkdir -p %s/class/gpio/gpio%u", root, 99) == 0);

    EXPECT(systemf("touch %s/class/gpio/export", root) == 0);
    EXPECT(systemf("touch %s/class/gpio/unexport", root) == 0);

    EXPECT(systemf("touch %s/class/gpio/gpio%u/direction", root, 99) == 0);
    EXPECT(systemf("touch %s/class/gpio/gpio%u/value", root, 99) == 0);
    EXPECT(systemf("touch %s/class/gpio/gpio%u/edge", root, 99) == 0);
    EXPECT(systemf("touch %s/class/gpio/gpio%u/active_low", root, 99) == 0);

    COMMENT("init");
    ASSERT(diminuto_modulator_init(&modulator, 99, 50) == &modulator);

        COMMENT("start");
        ASSERT(diminuto_modulator_start(&modulator) >= 0);

            diminuto_delay(diminuto_frequency(), 0);

            COMMENT("set");
            ASSERT(diminuto_modulator_set(&modulator, 20) >= 0);

            diminuto_delay(diminuto_frequency(), 0);

            COMMENT("set");
            ASSERT(diminuto_modulator_set(&modulator, 60) >= 0);

            diminuto_delay(diminuto_frequency(), 0);

        COMMENT("stop");
        ASSERT(diminuto_modulator_stop(&modulator) >= 0);

   COMMENT("fini");
    ASSERT(diminuto_modulator_fini(&modulator) == (diminuto_modulator_t *)0);

    EXPECT(systemf("rm -rf %s", root) == 0);

    prior = diminuto_pin_debug((const char *)0);
    ASSERT(prior != (const char *)0);
    EXPECT(prior == root);

    STATUS();

    EXIT();
}

