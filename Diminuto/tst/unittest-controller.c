/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * WORK IN PROGRESS!
 */

#include "com/diag/diminuto/diminuto_controller.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

static int16_t felapsed(void)
{
	return diminuto_frequency_ticks2units(diminuto_time_elapsed(), 1000LL);
}

static int16_t fdelay(int16_t ms)
{
	return diminuto_delay(diminuto_frequency_units2ticks(ms, 1000LL), 0);
}

static diminuto_controller_output_t fcontrol(const diminuto_controller_parameters_t * sp, diminuto_controller_state_t * dp, diminuto_controller_input_t target, diminuto_controller_output_t output, diminuto_controller_input_t threshold, int limit)
{
    int step = 0;
    diminuto_controller_input_t input;
    diminuto_controller_input_t before;
    diminuto_controller_input_t after;
    diminuto_controller_output_t slack;
    diminuto_controller_input_t offset;
    diminuto_controller_output_t prime;
    float error;

    printf("%4s %6s %6s %6s %6s %6s %6s %12s %12s %12s %12s %12s %6s\n", "STEP", "OUTPUT", "TARGET", "INPUT", "ERROR", "RATIO", "PRIME", "AVERAGE", "PROPORTIONAL", "INTEGRAL", "DIFFERENTIAL", "TOTAL", "DELTA");

    do {

        before = felapsed();
        slack = fdelay(output);
        assert(slack == 0);
        after = felapsed();

        input = after - before;
        offset = input - target;
        error = offset;
        error /= target;

        prime = diminuto_controller(sp, dp, target, input, output);

        printf("%4d %6d %6d %6d %6d %6.3f %6d %12d %12d %12d %12d %12d %6d\n", step++, output, target, input, offset, error, prime, dp->sample, dp->proportional, dp->integral, dp->differential, dp->total, dp->delta);
        assert(step < limit);

        output = prime;

    } while ((abs(offset) > threshold) || (dp->proportional != 0) || (dp->integral != 0) || (dp->differential != 0) || (dp->total != 0) || (dp->delta != 0));

    return output;
}

int main(int argc, char ** argv)
{
    {
        diminuto_controller_parameters_t parameters;
        diminuto_controller_state_t state;

        memset(&parameters, 0xAA, sizeof(parameters));
        memset(&state, 0x55, sizeof(state));

        diminuto_controller_init(&parameters, &state);

        assert(parameters.windup == (diminuto_controller_value_t)0x7fffffff);
        assert(parameters.minimum == (diminuto_controller_output_t)0x8000);
        assert(parameters.maximum == (diminuto_controller_output_t)0x7fff);
        assert(parameters.lower == (diminuto_controller_output_t)0x8000);
        assert(parameters.upper == (diminuto_controller_output_t)0x7fff);
        assert(parameters.kp.numerator == 1);
        assert(parameters.kp.denominator == 1);
        assert(parameters.ki.numerator == 1);
        assert(parameters.ki.denominator == 1);
        assert(parameters.kd.numerator == 1);
        assert(parameters.kd.denominator == 1);
        assert(parameters.kc.numerator == 1);
        assert(parameters.kc.denominator == 1);
        assert(parameters.filter == !0);

        assert(state.sample == 0);
        assert(state.proportional == 0);
        assert(state.integral == 0);
        assert(state.differential == 0);
        assert(state.total == 0);
        assert(state.delta == 0);
        assert(state.previous == 0);
        assert(state.initialized == 0);
    }

    {
        diminuto_controller_state_t state;

        memset(&state, 0x55, sizeof(state));

        diminuto_controller_init((diminuto_controller_parameters_t *)0, &state);

        assert(state.sample == 0);
        assert(state.proportional == 0);
        assert(state.integral == 0);
        assert(state.differential == 0);
        assert(state.total == 0);
        assert(state.delta == 0);
        assert(state.previous == 0);
        assert(state.initialized == 0);
    }

    {
        diminuto_controller_parameters_t parameters;

        memset(&parameters, 0xAA, sizeof(parameters));

        diminuto_controller_init(&parameters, (diminuto_controller_state_t *)0);

        assert(parameters.windup == (diminuto_controller_value_t)0x7fffffff);
        assert(parameters.minimum == (diminuto_controller_output_t)0x8000);
        assert(parameters.maximum == (diminuto_controller_output_t)0x7fff);
        assert(parameters.lower == (diminuto_controller_output_t)0x8000);
        assert(parameters.upper == (diminuto_controller_output_t)0x7fff);
        assert(parameters.kp.numerator == 1);
        assert(parameters.kp.denominator == 1);
        assert(parameters.ki.numerator == 1);
        assert(parameters.ki.denominator == 1);
        assert(parameters.kd.numerator == 1);
        assert(parameters.kd.denominator == 1);
        assert(parameters.kc.numerator == 1);
        assert(parameters.kc.denominator == 1);
        assert(parameters.filter == !0);
    }

    {
        diminuto_controller_init((diminuto_controller_parameters_t *)0, (diminuto_controller_state_t *)0);
    }

    {
        diminuto_controller_parameters_t parameters;
        diminuto_controller_state_t state;

        diminuto_controller_init(&parameters, &state);
        parameters.minimum = 0;
        parameters.lower = 0;
        fcontrol(&parameters, &state, 1000, 500, 1, 60);
    }

    {
        diminuto_controller_parameters_t parameters;
        diminuto_controller_state_t state;

        diminuto_controller_init(&parameters, &state);
        parameters.minimum = 0;
        parameters.lower = 0;
        fcontrol(&parameters, &state, 1000, 2000, 1, 60);
    }

#if 0
    {
        diminuto_controller_parameters_t parameters;
        diminuto_controller_state_t state;

        diminuto_controller_init(&parameters, &state);
        parameters.minimum = 0;
        parameters.lower = 0;
        parameters.kp = 0;
        parameters.ki = -1;
        parameters.kd = 0;
        parameters.filter = !0;
        fcontrol(&parameters, &state, 1000, 2000, 1, 60);
    }
#endif

    return 0;
}
