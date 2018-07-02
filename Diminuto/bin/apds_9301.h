/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_APDS_9301_
#define _H_COM_DIAG_DIMINUTO_APDS_9301_

/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * REFERENCES
 *
 * Avago, "APDS-9301 Miniature Ambient Light Photo Sensor with Digital
 * (I2C) Output", Avago Technologies, AV02-2315EN, 2010-01-07
 *
 * Wikipedia, "Lux", 2018-03-17
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <math.h>

/*
 * APDS-9301 data sheet, p. 4.
 */
static double apds_9310_chan2lux(uint16_t raw0, uint16_t raw1)
{
    double lux = 0.0;
    double chan0 = 0.0;
    double chan1 = 0.0;
    double ratio = 0.0;

    chan0 = raw0;
    chan1 = raw1;

    ratio = chan1 / chan0;

    if (ratio <= 0.50) {
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

    /*
     * $ luxcompute -p
     * luxcompute: 0x0 0x0 0.000000 0xffff 0x0 1992.264000
     */

    return lux;
}

#endif
