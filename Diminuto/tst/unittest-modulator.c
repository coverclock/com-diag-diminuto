/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corposcoren, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Modulator feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Modulator feature.
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

/*
 * This function tries to give a score to how well the modulator
 * on and off cycles are likely to work.
 * It is better for the on and off cycles to be close in value.
 * It is better for the on and off cycles to be small in value.
 * 100% on or 100% off are special cases.
 * Small scores are better.
 */
static unsigned int flicker(const diminuto_modulator_t * mp)
{
    double score;
    if (mp->toff == 0) {
        score = 0.0;
    } else if (mp->ton == 0) {
        score = 0.0;
    } else {
        double accum;
        accum = abs(mp->toff - mp->ton);
        accum /= 255.0;
        score = accum;
        accum = abs(mp->toff + mp->ton);
        accum /= 255.0;
        score += accum;
        score /= 2.0;
        score *= 100.0;
    }
    return (unsigned int)score;
}

int main(int argc, char ** argv)
{
    const char * root;
    const char * prior;
    char buffer[PATH_MAX];
    diminuto_modulator_t modulator;
    int duty;

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

    for (duty = 255; duty >= 0; --duty) {
        ASSERT(diminuto_modulator_init(&modulator, 99, duty) == &modulator);
        COMMENT("init duty %3d on %3u off %3u sum %3u mod %u flk %3u\n", modulator.duty, modulator.ton, modulator.toff, modulator.ton + modulator.toff, 255 % (modulator.ton + modulator.toff), flicker(&modulator));
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

        COMMENT("start");
        ASSERT(diminuto_modulator_error(&modulator) == 0);
        ASSERT(diminuto_modulator_start(&modulator) >= 0);
        diminuto_delay(diminuto_frequency(), 0);

            for (duty = 0; duty <= 255; ++duty) {
                ASSERT(diminuto_modulator_set(&modulator, duty) == 0);
                COMMENT("set duty %3d on %3u off %3u sum %3u mod %u flk %3u\n", modulator.duty, modulator.ton, modulator.toff, modulator.ton + modulator.toff, 255 % (modulator.ton + modulator.toff), flicker(&modulator));
                ASSERT(modulator.duty == duty);
                ASSERT(0 <= modulator.ton);
                ASSERT(modulator.ton <= 255);
                ASSERT(0 <= modulator.toff);
                ASSERT(modulator.toff <= 255);
                ASSERT(0 <= (modulator.ton + modulator.toff));
                ASSERT((modulator.ton + modulator.toff) <= 255);
                ASSERT((255 % (modulator.ton + modulator.toff)) == 0);
                diminuto_delay(diminuto_frequency(), 0);
            }

        COMMENT("stop");
        ASSERT(diminuto_modulator_stop(&modulator) >= 0);
        ASSERT(diminuto_modulator_error(&modulator) == 0);

    COMMENT("fini");
    ASSERT(diminuto_modulator_fini(&modulator) == (diminuto_modulator_t *)0);

    EXPECT(systemf("rm -rf %s", root) == 0);

    prior = diminuto_pin_debug((const char *)0);
    ASSERT(prior != (const char *)0);
    EXPECT(prior == root);

    STATUS();

    EXIT();
}

