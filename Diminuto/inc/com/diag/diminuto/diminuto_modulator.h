/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MODULATOR_
#define _H_COM_DIAG_DIMINUTO_MODULATOR_

/**
 * @file
 *
 * Copyright 2018-2019 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * Implements a pulse width modulation (PWM) generator ("modulator") in
 * software. By default controls a GPIO pin using the Diminuto pin feature,
 * but can be programmed to call a user-supplied function instead. The duty
 * cycle must be a value between 0 (fully off) and 100 (fully on). The duty
 * cycle can be modified as the PWM generator runs. Multiple PWM generators can
 * be active concurrently up to the limit of the CPU. Each generator runs a
 * thread-ish function every 100us (100000ns) i.e. 10000Hz. Has been
 * successfully run on a HW test fixture controlling four LEDs simultaneously.
 * No guarantees as to jitter, equivalent output voltage, or how it scales as
 * generators are added. See the functional test bin/pwmrheostat.c for a
 * working example.
 *
 * Important safety tip: I've worked with microcontrollers whose hardware
 * PWM generators had duty cycles in the range [0..255]. I thought about
 * implementing Modulator this way; it would have been easy. But using
 * the range [0..100] not only seems more intuitive (it's clearly a
 * percentage), but the value 100 yields more prime factors than 255,
 * which turns out to be useful in the implementation.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

/**
 * @def COM_DIAG_DIMINUTO_MODULATOR_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you chould always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_MODULATOR_FREQUENCY (10000LL)

/**
 * Returns the frequency of this feature: the number of cycles per second that
 * it can operate at. The inverse is the period of the PWM generator.
 * @return the frequency of this feature in Hertz.
 */
static inline diminuto_sticks_t diminuto_modulator_frequency(void)
{
    return COM_DIAG_DIMINUTO_MODULATOR_FREQUENCY; /* 100us or 100000ns */
}

/**
 * This is the data type large enough to hold a PWM duty cycle and
 * variables related to it. 
 */
typedef uint8_t diminuto_modulator_cycle_t;

/**
 * This is the smallest duty cycle value.
 */
static const diminuto_modulator_cycle_t DIMINUTO_MODULATOR_MINIMUM_DUTY = 0;

/**
 * This is the largest uty cycle value.
 */
static const diminuto_modulator_cycle_t DIMINUTO_MODULATOR_MAXIMUM_DUTY = 100;

/**
 * Defines the prototype for a PWM generator function.
 */
typedef void (diminuto_modulator_function_t)(union sigval);

/**
 * Defines the structure containing the state of a PWM generator.
 */
typedef struct DiminutoModulator {
    /* Fields for use by the application, otherwise unused. */
    void * data;
    /* Fields computed at Initialization or Finish. */
    diminuto_modulator_function_t * function;
    FILE * fp;
    int pin;
    /* Fields computed at Start or Stop. */
    timer_t timer;
    pthread_attr_t attributes;
    int initialized;
    /* Fields computed at Set. */
    volatile diminuto_modulator_cycle_t duty;
    volatile diminuto_modulator_cycle_t on;
    volatile diminuto_modulator_cycle_t off;
    volatile bool set;
    /* Fields computed at Run. */
    volatile diminuto_modulator_cycle_t total;
    volatile diminuto_modulator_cycle_t cycle;
    volatile diminuto_modulator_cycle_t ton;
    volatile diminuto_modulator_cycle_t toff;
    volatile bool condition;
} diminuto_modulator_t;

/**
 * Print the modulator structure.
 * @param fp points to the FILE to which the structure is printed.
 * @param mp points to the modulator structure.
 */
extern void diminuto_modulator_print(FILE * fp, const diminuto_modulator_t * mp);

/**
 * Initializes a modulator structure with a given function, a pin number, and
 * a duty cycle.
 * @param mp points to the modulator structure.
 * @param funp points to the modulator function.
 * @param pin is the GPIO pin number.
 * @param duty is the duty cycle in the range [0..100].
 * @return 0 for success, <0 if an error occured.
 */
extern int diminuto_modulator_init_generic(diminuto_modulator_t * mp, diminuto_modulator_function_t * funp, int pin, diminuto_modulator_cycle_t duty);

/**
 * Initializes a modulator structure with the default function, a pin number,
 * and a duty cycle.
 * @param mp points to the modulator structure.
 * @param pin is the GPIO pin number.
 * @param duty is the initial duty cycle in the range [0..100].
 * @return 0 for success, <0 if an error occured.
 */
static inline int diminuto_modulator_init(diminuto_modulator_t * mp, int pin, diminuto_modulator_cycle_t duty) {
    extern void diminuto_modulator_function(union sigval arg);
    return diminuto_modulator_init_generic(mp, diminuto_modulator_function, pin, duty);
}

/**
 * Starts a modulator.
 * @param mp points to the modulator structure.
 * @return 0 for success, <0 if an error occured.
 */
extern int diminuto_modulator_start(diminuto_modulator_t * mp);

/**
 * Changes the duty cycle of a modulator. Can be called any time after
 * initialization and before finishing, including while the modulator
 * is running. The modulator will assume the new duty cycle after the current
 * complete cycle of 100 firings has completed.
 * @param mp points to the modulator structure.
 * @param duty is the new duty cycle in the range [0..100].
 * @return 0 for success, <0 if an error occured.
 */
extern int diminuto_modulator_set(diminuto_modulator_t * mp, diminuto_modulator_cycle_t duty);

/**
 * Stops a modulator
 * @param mp points to the modulator structure.
 * @return 0 for success, <0 if an error occured.
 */
extern int diminuto_modulator_stop(diminuto_modulator_t * mp);

/**
 * Finishes a modulator structure by releasing any resources.
 * @param mp points to the modulator structure.
 * @return 0 for success, <0 if an error occured.
 */
extern int diminuto_modulator_fini(diminuto_modulator_t * mp);

#endif
