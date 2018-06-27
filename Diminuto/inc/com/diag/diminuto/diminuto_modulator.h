/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MODULATOR_
#define _H_COM_DIAG_DIMINUTO_MODULATOR_

/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include <pthread.h>
#include <stdio.h>

#define COM_DIAG_DIMINUTO_MODULATOR_FREQUENCY (10000LL)

static inline diminuto_sticks_t diminuto_modulator_frequency(void)
{
    return COM_DIAG_DIMINUTO_MODULATOR_FREQUENCY; /* 100us or 100000ns */
}

typedef struct DiminutoModulator {
	/* Fields initialized by timer at start. */
	timer_t timer;
	int initialized;
	/* Fields initialized by API at init. */
	int pin;
	int duty;
	FILE * fp;
	/* Fields computed by API at set. */
	int on;
	int off;
	int set;
	/* Fields computed by thread at notify. */
	int total;
	int cycle;
	int ton;
	int toff;
	int condition;
} diminuto_modulator_t;

extern int diminuto_modulator_init(diminuto_modulator_t * mp, int pin, int duty);

extern int diminuto_modulator_start(diminuto_modulator_t * mp);

extern int diminuto_modulator_set(diminuto_modulator_t * mp, int duty);

extern int diminuto_modulator_stop(diminuto_modulator_t * mp);

extern int diminuto_modulator_fini(diminuto_modulator_t * mp);

#endif
