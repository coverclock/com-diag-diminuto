/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * USAGE
 *
 * luxcompute CHAN0 CHAN1
 *
 * EXAMPLES
 *
 * luxcompute 0x0ef4 0x0393
 *
 * ABSTRACT
 *
 * Computes effective lux from the channel 0 (visible + infrared) and
 * channel 1 (infrared) values read from the APDS 9301 sensor.
 *
 * REFERENCES
 *
 * Avago, "APDS-9301 Miniature Ambient Light Photo Sensor with Digital
 * (I2C) Output", Avago Technologies, AV02-2315EN, 2010-01-07
 *
 * Wikipedia, "Lux", 2018-03-17
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <values.h>

static double apds_9310_chan2lux(uint16_t raw0, uint16_t raw1)
{
    double lux = 0.0;
    double chan0 = 0.0;
    double chan1 = 0.0;
    double ratio = 0.0;

    chan0 = raw0;
    chan1 = raw1;

    ratio = chan1 / chan0;

    if (chan0 <= 0.0) {
        lux = 0.0;
    } else if (ratio <= 0.50) {
        lux = (0.0304 * chan0) - (0.062 * chan1 * pow(ratio, 1.4));
    } else if ((0.50 < ratio) && (ratio <= 0.61)) {
        lux = (0.0224 * chan0) - (0.031 * chan1);
    } else if ((0.61 < ratio) && (ratio <= 0.80)) {
        lux = (0.0128 * chan0) - (0.0153 * chan1);
    } else if ((0.80 < ratio) && (ratio <= 1.30)) {
        lux = (0.00146 * chan0) - (0.00112 * chan1);
    } else {
        lux = 0.0;
    }

    return lux;
}

int main(int argc, char ** argv)
{
    int offset = 0;
    int debug = 0;
    const char * program = (const char *)0;
    char * endptr = (char *)0;
    uint16_t raw0 = 0;
    uint16_t raw1 = 0;
    double lux = 0.0;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    if (argc != 2) {
        /* Do nothing. */
    } else if (strncmp(argv[1], "-p", sizeof("-p")) != 0) {
        /* Do nothing. */
    } else {
        uint16_t minraw0 = 0;
        uint16_t maxraw0 = 0;
        uint16_t minraw1 = 0;
        uint16_t maxraw1 = 0;
        double min = MAXDOUBLE;
        double max = MINDOUBLE;
        raw0 = 0x0000;
        while (!0) {
            raw1 = 0x0000;
            while (!0) {
                lux = apds_9310_chan2lux(raw0, raw1);
                if (lux < min) { min = lux; minraw0 = raw0; minraw1 = raw1; }
                if (lux > max) { max = lux; maxraw0 = raw0; maxraw1 = raw1; }
                if (raw1 >= 0xffff) { break; }
                raw1 += 1;
            }
            if (raw0 >= 0xffff) { break; }
            raw0 += 1;
        }
        fprintf(stderr, "%s: 0x%x 0x%x %f 0x%x 0x%x %f\n", program, minraw0, minraw1,  min, maxraw0, maxraw1, max);
        return 0;
    }

    if (argc < 3) {
        fprintf(stderr, "usage: %s [ -p | [ -d ] CHAN0 CHAN1 ]\n", program);
        return 1;
    }

    if (strncmp(argv[1], "-d", sizeof("-d")) == 0) {
        debug = !0;
        offset = 1;
    }

	raw0 = strtoul(argv[offset + 1], &endptr, 0);
    if ((endptr == (char *)0) || (*endptr != '\0')) {
		errno = EINVAL;
		diminuto_perror(argv[offset + 1]);
		return 1;
    }

	raw1 = strtoul(argv[offset + 2], &endptr, 0);
    if ((endptr == (char *)0) || (*endptr != '\0')) {
		errno = EINVAL;
		diminuto_perror(argv[offset + 2]);
		return 1;
    }

    lux = apds_9310_chan2lux(raw0, raw1);

    if (debug) {
        fprintf(stderr, "%s: 0x%x %d 0x%x %d %f\n", program, raw0, raw0, raw1, raw1, lux);
    }

    printf("%0.2lf\n", lux);

    return 0;
}

