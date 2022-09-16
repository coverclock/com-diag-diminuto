/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MODULATOR_
#define _H_COM_DIAG_DIMINUTO_MODULATOR_

/**
 * @file
 * @copyright Copyright 2018-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a PWM generator.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Modulator feature implements a pulse width modulation (PWM) generator
 * in software. By default controls a GPIO pin using the Diminuto pin feature,
 * but can be programmed to call a user-supplied function instead. The duty
 * cycle must be a value between 0 (fully off) and 255 (fully on). The duty
 * cycle can be modified as the PWM generator runs. Multiple PWM generators can
 * be active concurrently up to the limit of the CPU. Each generator runs a
 * thread-ish function every 100us (100000ns) i.e. 10000Hz. Has been
 * successfully run on a HW test fixture controlling four LEDs simultaneously.
 *
 * No guarantees as to jitter, equivalent output voltage, or how it scales as
 * generators are added. See the functional test bin/pwmrheostat.c for a
 * working example.
 *
 * This has been an interesting experiment. But if I were building a product
 * where accurate PWM control was necessary, I would use a microcontroller with
 * a hardware PWM generator. I've successfully used eight-bit Atmel AVR and
 * Microchip PIC parts in this capacity, but there are lots of other choices.
 * But this is useful for LED control on an embedded Linux-based system.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_condition.h"
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
 * This the type of the modulator state (ARM, DISARM, IDLE), which
 * is the same as the underlying timer state.
 */
typedef enum DiminutoModulatorState {
    DIMINUTO_MODULATOR_STATE_IDLE       = DIMINUTO_TIMER_STATE_IDLE,
    DIMINUTO_MODULATOR_STATE_ARM        = DIMINUTO_TIMER_STATE_ARM,
    DIMINUTO_MODULATOR_STATE_DISARM     = DIMINUTO_TIMER_STATE_DISARM,
    DIMINUTO_MODULATOR_STATE_UNKNOWN    = DIMINUTO_TIMER_STATE_UNKNOWN,
} diminuto_modulator_state_t;

/**
 * This is the smallest duty cycle value.
 */
static const diminuto_modulator_cycle_t DIMINUTO_MODULATOR_DUTY_MIN = 0;

/**
 * This is the largest duty cycle value.
 */
static const diminuto_modulator_cycle_t DIMINUTO_MODULATOR_DUTY_MAX = 255;

/**
 * Defines the structure containing the state of a PWM generator.
 */
typedef struct DiminutoModulator {
    FILE *                      fp;         /**< GPIO /sys file pointer. */
    diminuto_timer_t            timer;      /**< Diminuto timer. */
    diminuto_condition_t        condition;  /**< Diminuto condition variable. */
    int32_t                     pin;        /**< GPIO /sys pin number. */
    int32_t                     error;      /**< Most recent error number. */
    diminuto_modulator_cycle_t  duty;       /**< Duty cycle [0..255]. */
    diminuto_modulator_cycle_t  on;         /**< Current on period [0..255]. */
    diminuto_modulator_cycle_t  off;        /**< Current off period [0..255]. */
    diminuto_modulator_cycle_t  cycle;      /**< Cycle countdown [0..255]. */
    diminuto_modulator_cycle_t  ton;        /**< On period [0..255]. */
    diminuto_modulator_cycle_t  toff;       /**< Off period [0..255]. */
    bool                        state;      /**< Output state: on or off. */
    bool                        set;        /**< Condition ready? (SHARED) */
} diminuto_modulator_t;

/**
 * Return the state of the modulator (ARM, DISARM, IDLE), which is the same
 * as the underlying timer state.
 */
extern diminuto_modulator_state_t diminuto_modulator_state(diminuto_modulator_t * mp);

/**
 * Remove the common prime factors from both the on and off cycles. This is
 * done in-place. This is called automatically by the set() function, but is
 * exposed for unit testing and other application uses.
 * @param onp points to the on cycle value.
 * @param offp points to the off cycle value.
 * @return 0 if not changed, !0 if changed.
 */
extern int diminuto_modulator_factor(diminuto_modulator_cycle_t * onp, diminuto_modulator_cycle_t * offp);

/**
 * Initializes a modulator structure with the default function, a pin number,
 * and a duty cycle. A duty cycle of 0 is fully off, and 255 is fully on.
 * @param mp points to the modulator structure.
 * @param pin is the GPIO pin number.
 * @param duty is the initial duty cycle in the range [0..255].
 * @return a pointer to the object for success, NULL if an error occurred.
 */
extern diminuto_modulator_t * diminuto_modulator_init(diminuto_modulator_t * mp, int pin, diminuto_modulator_cycle_t duty);

/**
 * Changes the duty cycle of a modulator. Can be called any time after
 * initialization and before finishing, including while the modulator
 * is running. The modulator will assume the new duty cycle after the current
 * complete cycle of 255 firings has completed. A duty cycle of 0 is fully
 * off, and 255 is fully on.
 * @param mp points to the modulator structure.
 * @param duty is the new duty cycle in the range [0..255].
 * @return 0 for success, <0 if an error occured.
 */
extern int diminuto_modulator_set(diminuto_modulator_t * mp, diminuto_modulator_cycle_t duty);

/*
 * This function tries to give a score to how well the modulator
 * on and off cycles are likely to work by computing a "flicker"
 * value between 0 and 100. SMALL scores are better. It uses
 * the transitional variables set by the set function.
 * It is better for the on and off cycles to be close in value.
 * It is better for the on and off cycles to be small in value.
 * 100% on or 100% off are special cases with perfect scores.
 * @param on is the on cycle value;
 * @param off is the off cycle value;
 * @return a flicker score between 0 (best) and 100 (worst).
 */
extern unsigned int diminuto_modulator_flicker(diminuto_modulator_cycle_t on, diminuto_modulator_cycle_t off);

/**
 * Starts a modulator.
 * @param mp points to the modulator structure.
 * @return 0 for success, <0 if an error occured.
 */
extern int diminuto_modulator_start(diminuto_modulator_t * mp);

/**
 * Return any error code produced during the execution of code in
 * support of the modulator callback function (for example:
 * a failure in a condition signal, or in pin control). This error
 * field is cleared when the modulator is initialized, and again
 * when it is started.
 * @param mp points to the modulator object.
 * @return an error code or zero if none.
 */
static inline int diminuto_modulator_error(const diminuto_modulator_t * mp)
{
    return mp->error;
}

/**
 * Stops a modulator. THis function blocks the caller until the
 * underlying Diminuto timer has acknowledged that is has been
 * disarmed.
 * @param mp points to the modulator structure.
 * @return 0 for success, <0 if an error occured.
 */
extern int diminuto_modulator_stop(diminuto_modulator_t * mp);

/**
 * Finishes a modulator structure by releasing any resources.
 * @param mp points to the modulator structure.
 * @return NULL for success, a pointer ot th eobject if an error occurred.
 */
extern diminuto_modulator_t * diminuto_modulator_fini(diminuto_modulator_t * mp);

#endif
