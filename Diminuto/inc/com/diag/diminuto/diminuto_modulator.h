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
#include "com/diag/diminuto/diminuto_pin.h"
#include <pthread.h>
#include <stdio.h>

typedef enum DiminutoModulatorState {
	DIMINUTO_MODULATOR_ZERO			= '\0',
	DIMINUTO_MODULATOR_READY		= 'I',
	DIMINUTO_MODULATOR_RUNNING		= 'R',
	DIMINUTO_MODULATOR_DONE			= 'D',
	DIMINUTO_MODULATOR_EXIT			= 'X',
} diminuto_modulator_state_t;

typedef struct DiminutoModulator {
	diminuto_modulator_state_t state;
	pthread_t thread;
	pthread_mutex_t mutex;
	int pin;
	FILE * fp;
	int duty;
	int on;
	int off;
} diminuto_modulator_t;

extern int diminuto_modulator_init(diminuto_modulator_t * mp, int pin, int duty);

extern int diminuto_modulator_start(diminuto_modulator_t * mp);

extern void diminuto_modulator_set(diminuto_modulator_t * mp, int duty);

extern int diminuto_modulator_stop(diminuto_modulator_t * mp);

extern int diminuto_modulator_fini(diminuto_modulator_t * mp);

#endif
