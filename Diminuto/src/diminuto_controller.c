/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Controller feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Controller feature.
 */

#include "com/diag/diminuto/diminuto_controller.h"
#include <stdlib.h>

diminuto_controller_state_t * diminuto_controller_init(diminuto_controller_parameters_t * sp, diminuto_controller_state_t * dp)
{

    if (sp != (diminuto_controller_parameters_t *)0) {
        sp->windup = DIMINUTO_CONTROLLER_MAXIMUM_VALUE;
        sp->minimum = DIMINUTO_CONTROLLER_MINIMUM_OUTPUT;
        sp->maximum = DIMINUTO_CONTROLLER_MAXIMUM_OUTPUT;
        sp->lower = DIMINUTO_CONTROLLER_MINIMUM_OUTPUT;
        sp->upper = DIMINUTO_CONTROLLER_MAXIMUM_OUTPUT;
        sp->kp.numerator = 1;
        sp->kp.denominator = 1;
        sp->ki.numerator = 1;
        sp->ki.denominator = 1;
        sp->kd.numerator = 1;
        sp->kd.denominator = 1;
        sp->kc.numerator = 1;
        sp->kc.denominator = 1;
        sp->filter = !0;
    }

    if (dp != (diminuto_controller_state_t *)0) {
        dp->sample = 0;
        dp->proportional = 0;
        dp->integral = 0;
        dp->differential = 0;
        dp->total = 0;
        dp->delta = 0;
        dp->previous = 0;
        dp->initialized = 0;
    }

    return dp;
}

void diminuto_controller_parameters_print(FILE * fp, const diminuto_controller_parameters_t * sp)
{
    fprintf(fp, "controller.parameters@0x%p[%zu]:"
                " windup=%d"
                " minimum=%d"
                " maximum=%d"
                " lower=%d"
                " upper=%d"
                " Kp=%d/%d"
                " Ki=%d/%d"
                " Kd=%d/%d"
                " Kc=%d/%d"
                " filter=%d"
                "\n",
        (void *)sp,
        sizeof(*sp),
        sp->windup,
        sp->minimum,
        sp->maximum,
        sp->lower,
        sp->upper,
        sp->kp.numerator, sp->kp.denominator,
        sp->ki.numerator, sp->ki.denominator,
        sp->kd.numerator, sp->kd.denominator,
        sp->kc.numerator, sp->kc.denominator,
        sp->filter
    );
}

void diminuto_controller_state_print(FILE * fp, const diminuto_controller_state_t * dp)
{
    fprintf(fp, "controller.state@0x%p[%zu]:"
                " sample=%d"
                " proportional=%d"
                " integral=%d"
                " differential=%d"
                " total=%d"
                " delta=%d"
                " previous=%d"
                " initialized=%d"
                "\n",
        (void *)dp,
        sizeof(dp),
        dp->sample,
        dp->proportional,
        dp->integral,
        dp->differential,
        dp->total,
        dp->delta,
        dp->previous,
        dp->initialized
    );
}

static inline diminuto_controller_value_t apply_gain(diminuto_controller_value_t value, const diminuto_controller_gain_t * gp)
{
    if (gp->denominator == 0) {
        value = DIMINUTO_CONTROLLER_MAXIMUM_VALUE;
    } else if (gp->numerator == 0) {
        value = 0;
    } else if (gp->numerator == gp->denominator) {
        /* Do nothing. */
    } else if (gp->denominator == 1) {
        value *= gp->numerator;
    } else if (gp->numerator == 1) {
        value /= gp->denominator;
    } else {
        value *= gp->numerator;
        value /= gp->denominator;
    }

    return value;
}

diminuto_controller_output_t diminuto_controller(const diminuto_controller_parameters_t * sp, diminuto_controller_state_t * dp, diminuto_controller_input_t target, diminuto_controller_input_t input, diminuto_controller_output_t output)
{
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

    dp->total = apply_gain(dp->proportional, &(sp->kp));

    /*
     * Compute the I term. We sum the integral post-gain instead of pre-gain to
     * reduce the effects of dynamic changes in Ki. We clamp the magnitude of
     * the I term to limit the integral windup. Note that the I coefficient is
     * typically a loss.
     */

    dp->integral += apply_gain(dp->proportional, &(sp->ki));

    if (dp->integral > sp->windup) {
        dp->integral = sp->windup;
    } else if (dp->integral < -(sp->windup)) {
        dp->integral = -(sp->windup);
    } else {
        /* Do nothing. */
    }

    dp->total += dp->integral;

    /*
     * Compute the D term. We limit the effect of the derivative kick by
     * implementing derivative on measurement. This subtracts the derivative
     * of the sample instead of adding the derivative of the error, which is
     * mathematically equivalent but causes fewer perturbations when the target
     * value changes. Note that the D coefficient is typically a gain.
     */

    dp->differential = dp->sample - dp->previous;

    dp->total -= apply_gain(dp->differential, &(sp->kd));

    dp->previous = dp->sample;

    /*
     * It's important that we do the conversion here on the delta and not
     * on the individual terms, because doing so the individual terms could have
     * reduced them each to zero, where as the sum of the three terms may not
     * be reduced to zero. Even this gain/loss can be disabled, effectively
     * disabling the entire controller.
     */

    dp->delta = apply_gain(dp->total, &(sp->kc));

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
