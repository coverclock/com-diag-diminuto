/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_TELEGRAPH_CONTROLLER_
#define _H_COM_DIAG_TELEGRAPH_CONTROLLER_

/**
 * @file
 *
 * Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Telegraph.html<BR>
 *
 * ABSTRACT
 *
 * Implement a generic Proportional/Integral/Differential (PID) controller.
 * This implementation goes to great lengths NOT to use multiply or divide, so
 * all coefficients are handled by bit shifts. Many eight-bit microcontrollers
 * lack floating point hardware, or even integer multiply and divide
 * instructions. Doing even integer multiplies and divides requires a function
 * library that may place the application outside of its memory footprint. This
 * isn't a theoretical concern; I've run into this on PIC microcontrollers for
 * example. Chances are good that this raw code won't meet your needs, since in
 * my personal experience, having written a few of these things, PID controllers
 * are application specific, finicky, and quick to anger. But this one at least
 * serves as a reference model from whence to implement a more tailored version.
 *
 * REFERENCES
 *
 * T. Wescott, "PID without a PhD", EETimes-India, 2000-10
 *
 * B. Beauregard, "Improving the Beginner's PID", Project Blog, 2011-04
 *
 * K. Astrom, T. Hagglund, "PID Controllers: Theory, Design, and Tuning",
 * 2nd ed., Instrumentation, Systems and Automation Society, 1995-01-01
 *
 * P. Albertos, I. Mareels, "Feedback and Control for Everyone", Springer, 2010
 */

#include "com/diag/telegraph/telegraph_types.h"

/**
 * This is the type of the input value (for example, a A/D sensor).
 * Change it if you need to.
 */
typedef int16_t telegraph_controller_input_t;

/**
 * This is the type of the output value (for example, a D/A actuator).
 * Change it if you need to.
 */
typedef int16_t telegraph_controller_output_t;

/**
 * This is the type used for persistent dynamic state variables.
 * Change it if you need to.
 */
typedef int32_t telegraph_controller_value_t;

/**
 * This is the type used for using shift values as gains (positive shift value),
 * and losses (negative shift value), which is much much faster and cheaper than
 * multiplication and division. As long as shifting works for you, you are
 * unlikely to need to change this type.
 */
typedef int8_t telegraph_controller_gain_t;

/**
 * Any absolute bit shift greater than or equal to this value indicates that
 * the value to which the gain or loss is being applied is effectively
 * eliminated. For example, this can be used to eliminate the use of the
 * differential term by setting Kd to this value.
 */
enum TelegraphControllerSymbols {
    DISABLED = (sizeof(telegraph_controller_value_t) * 8),
} telegraph_controller_symbols_t;

/**
 * This is the static state of the controller. It is not expected to change
 * while the controller is running. The application initializes them once, and
 * may in fact use the same structure for more than one active controllers.
 */
typedef struct TelegraphControllerParameters {

    /**
     * This is the limit on the windup on the integral term. It is applied
     * appropriately at the positive or negative end of the range. This limits
     * the memory of the integral term.
     */
    telegraph_controller_value_t windup;

    /**
     * This is the minimum value used to clamp the output at the low end. It
     * is the minimum legal output value.
     */
    telegraph_controller_output_t minimum;

    /**
     * This is the maximum value used to clamp the output at the high end. It
     * is the maximum legal output value.
     */
    telegraph_controller_output_t maximum;

    /**
     * Output values below this threshold are reduced to the minimum. Some
     * devices misbehave below a threshold, so if the output is less than this
     * value, it is set to the minimum value.
     */
    telegraph_controller_output_t lower;

    /**
     * Output values above this threshold are reduced to the maximum. Some
     * devices misbehave above a threshold, so if the output is more than this
     * value, it is set to the maximum value.
     */
    telegraph_controller_output_t upper;

    /**
     * This is the PID proportional shift value. It is in dimensionless units of
     * powers of two to indicate gain (or loss if negative). Kp IS TYPICALLY
     * POSITIVE INDICATING GAIN. The proportional term can be eliminated by
     * setting this field to the disabled value.
     */
    telegraph_controller_gain_t kp;

    /**
     * This is the PID integral shift value. It is in dimensionless units of
     * powers of two to indicate gain (or loss if negative). Ki IS TYPICALLY
     * NEGATIVE INDICATING LOSS. The integral term can be eliminated by setting
     * this field to the disabled value.
     */
    telegraph_controller_gain_t ki;

    /**
     * This is the PID differential shift value. It is in dimensionless units of
     * powers of two to indicate gain (or loss if negative). Kd IS TYPICALLY
     * POSITIVE INDICATING GAIN. The differential term can be eliminated by
     * setting this field to the disabled value.
     */
    telegraph_controller_gain_t kd;

    /**
     * This is the input to output conversion shift value to convert the
     * result of the PID computation on the input value into a suitable output
     * value. It is in dimensionless units of powers of two to indicate gain (or
     * loss if negative). The entire algorithm can be (perhaps temporarily)
     * disabled by setting this field to the disabled value. If the conversion
     * is more complicated than a simple bit shift, you can just specify zero
     * for this value, and the calling function can do the conversion on the
     * value that is returned by the controller function.
     */
    telegraph_controller_gain_t kc;

    /**
     * If true, this inverts the sign of the PID proportion term by subtracting
     * P instead of adding P. P IS TYPICALLY ADDED.
     */
    uint8_t sp;

    /**
     * If true, this inverts the sign of the PID integral term by subtracting I
     * instead of adding I. I IS TYPICALLY ADDED.
     */
    uint8_t si;

    /**
     * If true, invert the sign of the PID derivative term by adding D instead
     * of subtracting D. D IS TYPICALLY SUBTRACTED.
     */
    uint8_t sd;

    /**
     * If true, this applies a low pass filter to the initial input value to
     * smooth sudden brief transients.
     */
    uint8_t filter;

} telegraph_controller_parameters_t;

/**
 * This is the dynamic state of the controller. The application initializes one
 * of these for every individual controller, and each iteration of that
 * controller may change one or more of these values. The application can zero
 * out this structure in order to initialize it; the controller will take care
 * of everything else the first time it is called.
 */
typedef struct TelegraphControllerState {

    /**
     * This is a running average used to implement a low pass filter to ignore
     * sudden fast transients.
     */
    telegraph_controller_value_t sample;

    /**
     * This is the proportional term. It is persistent only to aid in debugging.
     */
    telegraph_controller_value_t proportional;

    /**
     * This is the integral term.
     */
    telegraph_controller_value_t integral;

    /**
     * This is the differential term. It is persistent only to aid in
     * debugging.
     */
    telegraph_controller_value_t differential;

    /**
     * This is the total of all three terms. It is persistent only to aid in
     * debugging.
     */
    telegraph_controller_value_t total;

    /**
     * This is the change in the output. It is persistent only to aid in
     * debugging.
     */
    telegraph_controller_output_t delta;

    /**
     * This is the previous output term.
     */
    telegraph_controller_input_t previous;

    /**
     * If false this causes the dynamic state structure to be reinitialized.
     */
    uint8_t initialized;

} telegraph_controller_state_t;

/**
 * Initialize a parameter structure and a state structure to useful default
 * values.
 * @param sp points to the static parameters structure or null if none.
 * @param dp points to the dynamic state structure or null if none.
 */
void telegraph_controller_init(telegraph_controller_parameters_t * sp, telegraph_controller_state_t * dp);

/**
 * Implement a left (gain) shift for a positive shift value or a right (loss)
 * shift for negative shift value while rounding towards zero when both the
 * datum value and the shift value are negative. (This is where it would be
 * really great if the ANSI C standard specified that all implementations must
 * implement right shifts for negative shift values.) This is used internally
 * in the controller, but it's a useful algorithm so it is exposed as part of
 * the public API.
 * @param value is the datum to be shifted.
 * @param kn is the shift value in bits.
 */
telegraph_controller_value_t telegraph_controller_shift(telegraph_controller_value_t value, telegraph_controller_gain_t kn);

/**
 * Advance the controller by one time step, recalculating the new output based
 * on the the target, the latest input, and the previous output. The
 * controller assumes that each time step is equal and monotonically increasing.
 * @param sp points to the static parameters structure.
 * @param dp points to the dynamic state structure.
 * @param target is the desired input value.
 * @param input is the actual input value (e.g. feedback from a sensor).
 * @param output is the output value that generated the input value.
 * @return the next output value.
 */
telegraph_controller_output_t telegraph_controller_step(const telegraph_controller_parameters_t * sp, telegraph_controller_state_t * dp, telegraph_controller_input_t target, telegraph_controller_input_t input, telegraph_controller_output_t output);

#endif
