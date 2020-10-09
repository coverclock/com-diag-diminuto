/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock<BR>
 * mailto:coverclock@diag.com<BR>
 * https://github.com/coverclock/com-diag-telegraph<BR>
 * WORK IN PROGRESS!
 */

#include "com/diag/telegraph/telegraph_controller.h"

static const telegraph_controller_value_t MAXIMUM_VALUE = (telegraph_controller_value_t)~(1 << ((sizeof(telegraph_controller_value_t) * 8) - 1));
static const telegraph_controller_output_t MINIMUM_OUTPUT = (telegraph_controller_output_t)(1 << ((sizeof(telegraph_controller_output_t) * 8) - 1));
static const telegraph_controller_output_t MAXIMUM_OUTPUT = (telegraph_controller_output_t)~(1 << ((sizeof(telegraph_controller_output_t) * 8) - 1));

void telegraph_controller_init(
    telegraph_controller_parameters_t * sp,
    telegraph_controller_state_t * dp
) {

    if (sp != (telegraph_controller_parameters_t *)0) {
        sp->windup = MAXIMUM_VALUE;
        sp->minimum = MINIMUM_OUTPUT;
        sp->maximum = MAXIMUM_OUTPUT;
        sp->lower = MINIMUM_OUTPUT;
        sp->upper = MAXIMUM_OUTPUT;
        sp->kp = 0;
        sp->ki = 0;
        sp->kd = 0;
        sp->kc = 0;
        sp->sp = 0;
        sp->si = 0;
        sp->sd = 0;
        sp->filter = !0;
    }

    if (dp != (telegraph_controller_state_t *)0) {
        dp->sample = 0;
        dp->proportional = 0;
        dp->integral = 0;
        dp->differential = 0;
        dp->total = 0;
        dp->delta = 0;
        dp->previous = 0;
        dp->initialized = 0;
    }

}

telegraph_controller_value_t telegraph_controller_shift(
    telegraph_controller_value_t value,
    telegraph_controller_gain_t kn
) {
    if (kn >= DISABLED) {
        value = 0;
    } else if (kn > 0) {
        value <<= kn;
    } else if (kn < 0) {
        kn = -kn;
        if (value < 0) {
            value += (((telegraph_controller_value_t)1) << kn) - 1;
        }
        value >>= kn;
    } else {
        /* Do nothing: (kn == 0). */
    }

    return value;
}

telegraph_controller_output_t telegraph_controller(
    const telegraph_controller_parameters_t * sp,
    telegraph_controller_state_t * dp,
    telegraph_controller_input_t target,
    telegraph_controller_input_t input,
    telegraph_controller_output_t output
) {
    telegraph_controller_value_t value;

    /*
     * If this is the first time through, initialize the PID controller.
     * Regarding the elapsed time since the last computation: dt here a constant
     * based on the sampling period of the input. We call dt unity based
     * on the sampling period being our unit of measure for time. So this
     * function must be called only when we have a new input, which must occur
     * on a consistently periodic basis. The caller can reset the algorithm
     * back to the beginning of time just by clearing the initialized field.
     */

    if (!dp->initialized) {
        dp->integral = 0;
        dp->previous = input;
        dp->sample = input;
        dp->initialized = !0;
    }

    /*
     * Apply a low pass filter to the sample to lessen the effect of fast
     * transients.
     */

    if (sp->filter) {
        dp->sample += input;
        dp->sample >>= 1;
    } else {
        dp->sample = input;
    }

    /*
     * Compute the P term. The P term is the error, which can be positive or
     * negative. Note that the P coefficient is typically a gain. The
     * proportional calculation is used in both the P term and the I term.
     */

    dp->proportional = target;
    dp->proportional -= dp->sample;

    value = telegraph_controller_shift(dp->proportional, sp->kp);

    if (sp->sp) {
        value = -value;
    }

    dp->total = value;

    /*
     * Compute the I term. We sum the integral post-gain instead of pre-gain to
     * reduce the effects of dynamic changes in Ki. We clamp the magnitude of
     * the I term to limit the integral windup. Note that the I coefficient is
     * typically a loss.
     */

    dp->integral += telegraph_controller_shift(dp->proportional, sp->ki);

    if (dp->integral > sp->windup) {
        dp->integral = sp->windup;
    } else if (dp->integral < -(sp->windup)) {
        dp->integral = -(sp->windup);
    } else {
        /* Do nothing. */
    }

    value = dp->integral;

    if (sp->si) {
        value = -value;
    }

    dp->total += value;

    /*
     * Compute the D term. We limit the effect of the derivative kick by
     * implementing derivative on measurement. This subtracts the derivative
     * of the sample instead of adding the derivative of the error, which is
     * mathematically equivalent but causes fewer perturbations when the target
     * value changes. Note that the D coefficient is typically a gain.
     */

    dp->differential = dp->sample - dp->previous;

    value = telegraph_controller_shift(dp->differential, sp->kd);

    if (sp->sd) {
    	value = -value;
    }

    dp->total -= value;

    dp->previous = dp->sample;

    /*
     * It's important that we do the conversion shift here on the delta and not
     * on the individual terms, because doing so the individual terms could have
     * reduced them each to zero, where as the sum of the three terms may not
     * be reduced to zero. Even this gain/loss can be disabled, effectively
     * disabling the entire controller.
     */

    dp->delta = telegraph_controller_shift(dp->total, sp->kc);
    output += dp->delta;

    /*
     * Clamp the output. We clamp the output even if the delta is zero
     * because the limits may have been updated, resulting in a different value
     * for the output.
     */

    if (output < sp->minimum) {
        output = sp->minimum;
    } else if (output > sp->maximum) {
        output = sp->maximum;
    } else {
        /* Do nothing. */
    }

    /*
     * Devices sometimes have "no fly zones" near their limits of operation.
     * So we check to see if we are in those zones, and if we are, we increase
     * the output to the maximum (above the upper limit) or decrease it to the
     * minimum (below the lower limit). (Example: when controlling the ballast
     * on a fluorescent light, the bulb flickers when it gets near zero; better
     * to just turned it off.
     */

    if ((sp->minimum < output) && (output < sp->lower)) {
        output = sp->minimum;
    } else if ((sp->upper < output) && (output < sp->maximum)) {
        output = sp->maximum;
    } else {
        /* Do nothing. */
    }

    return output;
}
