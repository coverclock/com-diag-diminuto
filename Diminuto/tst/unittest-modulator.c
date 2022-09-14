/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2021 Digital Aggregates Corposcoren, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Modulator feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Modulator feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_types.h"
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

static const char TMP[] = "/tmp";


static int systemf(const char * format, ...)
{
    int rc;
    va_list ap;
    diminuto_path_t buffer;

    COMMENT("format=\"%s\"", format);

    va_start(ap, format);
    vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    COMMENT("command=\"%s\"", buffer);

    ASSERT((rc = system(buffer)) >= 0);

    return rc;
}

int main(int argc, char ** argv)
{
    const char * root;
    const char * prior;
    diminuto_path_t buffer;
    diminuto_modulator_t modulator;
    int duty;
    unsigned int flicker;

    SETLOGMASK();

    TEST();

    COMMENT("sizeof=%zu", sizeof(buffer));
    ASSERT(argv[0] != (char *)0);
    root = strrchr(argv[0], '/');
    root = (root == (char *)0) ? argv[0] : root + 1;
    ASSERT(root != (const char *)0);
    COMMENT("root=\"%s\"", root);
    memset(&buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s/%s-XXXXXX", TMP, root);
    ASSERT(buffer[sizeof(buffer) - 1] == '\0');
    COMMENT("buffer=\"%s\"", buffer);
    root = mkdtemp(buffer);
    ASSERT(root != (const char *)0);
    ASSERT(*root != '\0');
    COMMENT("mkdtemp=\"%s\"", root);
    prior = diminuto_pin_debug(root);
    ASSERT(prior != (const char *)0);
    ASSERT(*prior != '\0');
    COMMENT("prior=\"%s\"", prior);

    EXPECT(systemf("mkdir -p %s/class/gpio/gpio%u", root, 99) == 0);

    EXPECT(systemf("touch %s/class/gpio/export", root) == 0);
    EXPECT(systemf("touch %s/class/gpio/unexport", root) == 0);

    EXPECT(systemf("touch %s/class/gpio/gpio%u/direction", root, 99) == 0);
    EXPECT(systemf("touch %s/class/gpio/gpio%u/value", root, 99) == 0);
    EXPECT(systemf("touch %s/class/gpio/gpio%u/edge", root, 99) == 0);
    EXPECT(systemf("touch %s/class/gpio/gpio%u/active_low", root, 99) == 0);

    CHECKPOINT("INIT");

    for (duty = 255; duty >= 0; --duty) {
        ASSERT(diminuto_modulator_init(&modulator, 99, duty) == &modulator);
        flicker = diminuto_modulator_flicker(&modulator);
        ASSERT(0 <= flicker);
        ASSERT(flicker <= 100);
        CHECKPOINT("init duty %3d on %3u off %3u sum %3u mod %u flk %3u\n", modulator.duty, modulator.ton, modulator.toff, modulator.ton + modulator.toff, 255 % (modulator.ton + modulator.toff), flicker);
        ASSERT(modulator.duty == duty);
        ASSERT(0 <= modulator.ton);
        ASSERT(modulator.ton <= 255);
        ASSERT(0 <= modulator.toff);
        ASSERT(modulator.toff <= 255);
        ASSERT(0 <= (modulator.ton + modulator.toff));
        ASSERT((modulator.ton + modulator.toff) <= 255);
        ASSERT((255 % (modulator.ton + modulator.toff)) == 0);
        if (duty > 0) {
            ASSERT(diminuto_modulator_fini(&modulator) == (diminuto_modulator_t *)0);;
        }
    }

    CHECKPOINT("START");

    ASSERT(diminuto_modulator_error(&modulator) == 0);
    ASSERT(diminuto_modulator_start(&modulator) >= 0);
    diminuto_delay(diminuto_frequency(), 0);

    CHECKPOINT("SET");

    for (duty = 0; duty <= 255; ++duty) {
        ASSERT(diminuto_modulator_set(&modulator, duty) == 0);
        flicker = diminuto_modulator_flicker(&modulator);
        ASSERT(0 <= flicker);
        ASSERT(flicker <= 100);
        CHECKPOINT("set duty %3d on %3u off %3u sum %3u mod %u flk %3u\n", modulator.duty, modulator.ton, modulator.toff, modulator.ton + modulator.toff, 255 % (modulator.ton + modulator.toff), flicker);
        ASSERT(0 <= modulator.ton);
        ASSERT(modulator.ton <= 255);
        ASSERT(0 <= modulator.toff);
        ASSERT(modulator.toff <= 255);
        ASSERT(0 <= (modulator.ton + modulator.toff));
        ASSERT((modulator.ton + modulator.toff) <= 255);
        ASSERT((255 % (modulator.ton + modulator.toff)) == 0);
        diminuto_delay(diminuto_frequency(), 0);
    }

    CHECKPOINT("STOP");

    ASSERT(diminuto_modulator_stop(&modulator) >= 0);
    ASSERT(diminuto_modulator_error(&modulator) == 0);

    ASSERT(diminuto_modulator_fini(&modulator) == (diminuto_modulator_t *)0);

    EXPECT(systemf("rm -rf %s", root) == 0);

    prior = diminuto_pin_debug((const char *)0);
    ASSERT(prior != (const char *)0);
    EXPECT(prior == root);

    STATUS();

    EXIT();
}

