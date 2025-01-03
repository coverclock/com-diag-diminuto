/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CONTROLLER_
#define _H_COM_DIAG_DIMINUTO_CONTROLLER_

/**
 * @file
 * @copyright Copyright 2014-2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a PID controller.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Controller feature is a generic Proportional-Integral-Differential
 * (PID) controller. It is based on a commercial microcontroller implementation
 * the author developed, but is a complete clean-room reimplementation.
 * This implementation goes to great lengths NOT to use floating point. It
 * does however use integer multiply and divide, unlike the similar feature
 * from which it was ported in the Telegraph library of features intended for
 * smaller microcontrollers. It has been functionally tested using a feedback
 * loop consisting of a PWM controlled LED and a light sensor.
 *
 * REFERENCES
 *
 * T. Wescott, "PID without a PhD", Embedded Systems Programming, 2000-10-01
 * (RECOMMENDED)
 *
 * B. Beauregard, "Improving the Beginner's PID", Project Blog, 2011-04
 *
 * K. Astrom, T. Hagglund, "PID Controllers: Theory, Design, and Tuning",
 * 2nd ed., Instrumentation, Systems and Automation Society, 1995-01-01
 *
 * P. Albertos, I. Mareels, "Feedback and Control for Everyone", Springer, 2010
 *
 * Wikipedia, "PID controller", 2018-06-27
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>

/**
 * @def COM_DIAG_DIMINUTO_CONTROLLER_MAXIMUM_VALUE
 * Defines the maximum possible controller value.
 */
#define COM_DIAG_DIMINUTO_CONTROLLER_MAXIMUM_VALUE ((diminuto_controller_value_t)(~(((diminuto_controller_value_t)1) << ((sizeof(diminuto_controller_value_t) * 8) - 1))))

/**
 * @def COM_DIAG_DIMINUTO_CONTROLLER_MINIMUM_OUTPUT
 * Defines the mininum possible controller output.
 */
#define COM_DIAG_DIMINUTO_CONTROLLER_MINIMUM_OUTPUT ((diminuto_controller_output_t)((((diminuto_controller_output_t)1) << ((sizeof(diminuto_controller_output_t) * 8) - 1))))

/**
 * @def COM_DIAG_DIMINUTO_CONTROLLER_MAXIMUM_OUTPUT
 * Defines the maximum possible controller output.
 */
#define COM_DIAG_DIMINUTO_CONTROLLER_MAXIMUM_OUTPUT ((diminuto_controller_output_t)(~(((diminuto_controller_output_t)1) << ((sizeof(diminuto_controller_output_t) * 8) - 1))))

/*
 * You can change these data types as you see fit. I chose them to
 * deliberately limit the dynamic range supported by the PID algorithm,
 * and to reduce the chances of overflowing an intermediate variable.
 * Using other types (including floating point) requires some serious
 * systems engineering thinking.
 */

/**
 * This is the type of input values (for example, a A/D sensor).
 */
typedef int16_t diminuto_controller_input_t;

/**
 * This is the type of output values (for example, a D/A actuator).
 */
typedef int16_t diminuto_controller_output_t;

/**
 * This is the type used for numerator and denominator gain values.
 */
typedef int16_t diminuto_controller_factor_t;

/**
 * This is the type used for intermediate computation variables.
 */
typedef int32_t diminuto_controller_value_t;

/**
 * This is the maximum value the value type can assume.
 */
static const diminuto_controller_value_t DIMINUTO_CONTROLLER_MAXIMUM_VALUE = COM_DIAG_DIMINUTO_CONTROLLER_MAXIMUM_VALUE;

/**
 * This is the minimum value the output type can assume.
 */
static const diminuto_controller_output_t DIMINUTO_CONTROLLER_MINIMUM_OUTPUT = COM_DIAG_DIMINUTO_CONTROLLER_MINIMUM_OUTPUT;

/**
 * This is the maximum value the output type can assume.
 */
static const diminuto_controller_output_t DIMINUTO_CONTROLLER_MAXIMUM_OUTPUT = COM_DIAG_DIMINUTO_CONTROLLER_MAXIMUM_OUTPUT;

/**
 * This is the structure used for using ratios as gains (numerator greater than
 * denominator), and losses (numerator less than denominator), which is much
 * much faster and cheaper than using floating point.
 */
typedef struct DiminutoControllerGain {

    /**
     * This is the numerator. If it is greater than the denominator, this is a
     * gain. If it is less, a loss. If it is zero, the term to which it is
     * applied is eliminated. If it is negative (and the denominator is not),
     * the effect of the term is inverted.
     */
    diminuto_controller_factor_t  numerator;

    /**
     * This is the denominator.
     */
    diminuto_controller_factor_t  denominator;

} diminuto_controller_gain_t;

/**
 * This is the static state of the controller. It is not expected to change
 * while the controller is running. The application initializes them once, and
 * may in fact use the same structure for more than one active controllers.
 */
typedef struct DiminutoControllerParameters {

    /**
     * This is the limit on the windup on the integral term. It is applied
     * appropriately at the positive or negative end of the range. This limits
     * the memory of the integral term.
     */
    diminuto_controller_value_t windup;

    /**
     * This is the minimum value used to clamp the output at the low end. It
     * is the minimum legal output value.
     */
    diminuto_controller_output_t minimum;

    /**
     * This is the maximum value used to clamp the output at the high end. It
     * is the maximum legal output value.
     */
    diminuto_controller_output_t maximum;

    /**
     * Output values below this threshold are reduced to the minimum. Some
     * devices misbehave below a threshold, so if the output is less than this
     * value, it is set to the minimum value.
     */
    diminuto_controller_output_t lower;

    /**
     * Output values above this threshold are reduced to the maximum. Some
     * devices misbehave above a threshold, so if the output is more than this
     * value, it is set to the maximum value.
     */
    diminuto_controller_output_t upper;

    /**
     * This is the PID proportional ratio value. It is in dimensionless units
     * to indicate gain (or loss if less than zero). Kp IS TYPICALLY
     * GREATER THAN ZERO INDICATING GAIN. The proportional term can be
     * eliminated by setting the numerator of this field to the zero.
     */
    diminuto_controller_gain_t kp;

    /**
     * This is the PID integral ratio value. It is in dimensionless units
     * to indicate gain (or loss if less than zero). Ki IS TYPICALLY
     * LESS THAN ZERO INDICATING LOSS. The integral term can be eliminated by
     * setting the numerator of this field to zero.
     */
    diminuto_controller_gain_t ki;

    /**
     * This is the PID differential ratio value. It is in dimensionless units
     * to indicate gain (or loss if less than zero). Kd IS TYPICALLY
     * GREATER THAN ZERO INDICATING GAIN. The differential term can be
     * eliminated by setting the numerator of this field to zero.
     */
    diminuto_controller_gain_t kd;

    /**
     * This is the input to output conversion ratio value to convert the
     * result of the PID computation on the input value into a suitable output
     * value. It is in dimensionless units to indicate gain (or loss if less
     * than zero). The entire algorithm can be (perhaps temporarily)
     * disabled by setting the numerator of this field to zero. It is better
     * to factor common factors out of Kp, Ki and Kd and apply them to the final
     * result. Kc is that common factor.
     */
    diminuto_controller_gain_t kc;

    /**
     * If true, this applies a low pass filter to the initial input value to
     * smooth sudden brief transients.
     */
    bool filter;

} diminuto_controller_parameters_t;

/**
 * Print a static parameters structure.
 * @param fp points to a FILE object.
 * @param sp points to a parameters structure.
 */
extern void diminuto_controller_parameters_print(FILE * fp, const diminuto_controller_parameters_t * sp);

/**
 * This is the dynamic state of the controller. The application initializes one
 * of these for every individual controller, and each iteration of that
 * controller may change one or more of these values. The application can zero
 * out this structure in order to initialize it; the controller will take care
 * of everything else the first time it is called.
 */
typedef struct DiminutoControllerState {

    /**
     * This is a running average used to implement a low pass filter to ignore
     * sudden fast transients.
     */
    diminuto_controller_value_t sample;

    /**
     * This is the proportional term. It is persistent only to aid in debugging.
     */
    diminuto_controller_value_t proportional;

    /**
     * This is the integral term.
     */
    diminuto_controller_value_t integral;

    /**
     * This is the differential term. It is persistent only to aid in
     * debugging.
     */
    diminuto_controller_value_t differential;

    /**
     * This is the total of all three terms. It is persistent only to aid in
     * debugging.
     */
    diminuto_controller_value_t total;

    /**
     * This is the change in the output. It is persistent only to aid in
     * debugging.
     */
    diminuto_controller_value_t delta;

    /**
     * This is the previous output term.
     */
    diminuto_controller_input_t previous;

    /**
     * If false this causes the dynamic state structure to be reinitialized.
     */
    bool initialized;

} diminuto_controller_state_t;

/**
 * Print a dynamic state structure.
 * @param fp points to a FILE object.
 * @param dp points to a state structure.
 */
extern void diminuto_controller_state_print(FILE * fp, const diminuto_controller_state_t * dp);

/**
 * Initialize a parameter structure and a state structure to useful default
 * values.
 * @param sp points to the static parameters structure or null if none.
 * @param dp points to the dynamic state structure or null if none.
 * @return a pointer to the stae object or NULL if an error occurred.
 */
extern diminuto_controller_state_t * diminuto_controller_init(diminuto_controller_parameters_t * sp, diminuto_controller_state_t * dp);

/**
 * Advance the controller by one time step, recalculating the new output based
 * on the the target, the latest input, and the previous output. The
 * controller assumes that each time step is equal and monotonically increasing.
 * So the algorithm assumes each call has the same sample time or dT, and that
 * dT is scaled to unity.
 * @param sp points to the static parameters structure.
 * @param dp points to the dynamic state structure.
 * @param target is the desired input value.
 * @param input is the actual input value (e.g. feedback from a sensor).
 * @param output is the output value that generated the input value.
 * @return the next output value.
 */
extern diminuto_controller_output_t diminuto_controller(const diminuto_controller_parameters_t * sp, diminuto_controller_state_t * dp, diminuto_controller_input_t target, diminuto_controller_input_t input, diminuto_controller_output_t output);

/**
 * Releases any dynamically acquired resources associated with a controller.
 * @param sp points to the static parameters structure or null if none.
 * @param dp points to the dynamic state structure or null if none.
 * @return NULL or a pointer to the state object if an error occurred.
 */
static inline diminuto_controller_state_t * diminuto_controller_fini(diminuto_controller_parameters_t * sp, diminuto_controller_state_t * dp)
{
    return (diminuto_controller_state_t *)0;
}

#endif
