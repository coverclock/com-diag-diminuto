/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CONTROLLER_
#define _H_COM_DIAG_DIMINUTO_CONTROLLER_

/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * WORK IN PROGRESS!
 *
 * ABSTRACT
 *
 * Implement a generic Proportional-Integral-Differential (PID) controller.
 * This implementation goes to great lengths NOT to use floating point. It
 * does however use integer multiply and divide, unlike the similar feature
 * from which it was ported in the Telegraph library of features intended for
 * smaller microcontrollers.
 *
 * REFERENCES
 *
 * T. Wescott, "PID without a PhD", Embedded Systems Programming, 2000-10-01
 *
 * B. Beauregard, "Improving the Beginner's PID", Project Blog, 2011-04
 *
 * K. Astrom, T. Hagglund, "PID Controllers: Theory, Design, and Tuning",
 * 2nd ed., Instrumentation, Systems and Automation Society, 1995-01-01
 *
 * P. Albertos, I. Mareels, "Feedback and Control for Everyone", Springer, 2010
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * This is the type of the input value (for example, a A/D sensor).
 */
typedef int16_t diminuto_controller_input_t;

/**
 * This is the type of the output value (for example, a D/A actuator).
 */
typedef int16_t diminuto_controller_output_t;

/**
 * This is the type used for persistent dynamic state variables.
 */
typedef int32_t diminuto_controller_value_t;

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
	int8_t numerator;

	/**
	 * This is the denominator.
	 */
	int8_t denominator;

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
    uint8_t filter;

} diminuto_controller_parameters_t;

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
    diminuto_controller_output_t delta;

    /**
     * This is the previous output term.
     */
    diminuto_controller_input_t previous;

    /**
     * If false this causes the dynamic state structure to be reinitialized.
     */
    uint8_t initialized;

} diminuto_controller_state_t;

/**
 * Initialize a parameter structure and a state structure to useful default
 * values.
 * @param sp points to the static parameters structure or null if none.
 * @param dp points to the dynamic state structure or null if none.
 */
void diminuto_controller_init(
    diminuto_controller_parameters_t * sp,
    diminuto_controller_state_t * dp
);

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
diminuto_controller_output_t diminuto_controller_step(
    const diminuto_controller_parameters_t * sp,
    diminuto_controller_state_t * dp,
    diminuto_controller_input_t target,
    diminuto_controller_input_t input,
    diminuto_controller_output_t output
);

#endif
