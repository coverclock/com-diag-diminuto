/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Controller feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Controller feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_controller.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
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
    int done = 0;
    int step = 0;
    diminuto_controller_input_t input;
    diminuto_controller_input_t before;
    diminuto_controller_input_t after;
    diminuto_controller_output_t slack;
    diminuto_controller_input_t offset;
    diminuto_controller_output_t prime;
    float error;

    CHECKPOINT("%4s %6s %6s %6s %6s %6s %6s %12s %12s %12s %12s %12s %6s\n", "STEP", "OUTPUT", "TARGET", "INPUT", "ERROR", "RATIO", "PRIME", "AVERAGE", "PROPORTIONAL", "INTEGRAL", "DIFFERENTIAL", "TOTAL", "DELTA");

    do {

        before = felapsed();
        slack = fdelay(output);
        ASSERT(slack == 0);
        after = felapsed();

        input = after - before;
        offset = input - target;
        error = offset;
        error /= target;

        prime = diminuto_controller(sp, dp, target, input, output);

        CHECKPOINT("%4d %6d %6d %6d %6d %6.3f %6d %12d %12d %12d %12d %12d %6d\n", step++, output, target, input, offset, error, prime, dp->sample, dp->proportional, dp->integral, dp->differential, dp->total, dp->delta);
        ASSERT(step < limit);

        output = prime;

        if (done) {
            break;
        }

        if ((abs(offset) <= threshold) && (dp->proportional == 0) && (dp->integral == 0) && (dp->differential == 0) && (dp->total == 0) && (dp->delta == 0)) {
            done = !0;
        }

    } while (!0);

    return output;
}

int main(int argc, char ** argv)
{

    SETLOGMASK();

    {
        diminuto_controller_parameters_t parameters;
        diminuto_controller_state_t state;

        TEST();

        memset(&parameters, 0xAA, sizeof(parameters));
        memset(&state, 0x55, sizeof(state));

        ASSERT(diminuto_controller_init(&parameters, &state) == &state);

        diminuto_controller_parameters_print(stdout, &parameters);
        diminuto_controller_state_print(stdout, &state);

        ASSERT(parameters.windup == DIMINUTO_CONTROLLER_MAXIMUM_VALUE);
        ASSERT(parameters.minimum == DIMINUTO_CONTROLLER_MINIMUM_OUTPUT);
        ASSERT(parameters.maximum == DIMINUTO_CONTROLLER_MAXIMUM_OUTPUT);
        ASSERT(parameters.lower == DIMINUTO_CONTROLLER_MINIMUM_OUTPUT);
        ASSERT(parameters.upper == DIMINUTO_CONTROLLER_MAXIMUM_OUTPUT);
        ASSERT(parameters.kp.numerator == 1);
        ASSERT(parameters.kp.denominator == 1);
        ASSERT(parameters.ki.numerator == 1);
        ASSERT(parameters.ki.denominator == 1);
        ASSERT(parameters.kd.numerator == 1);
        ASSERT(parameters.kd.denominator == 1);
        ASSERT(parameters.kc.numerator == 1);
        ASSERT(parameters.kc.denominator == 1);
        ASSERT(parameters.filter == !0);

        ASSERT(state.sample == 0);
        ASSERT(state.proportional == 0);
        ASSERT(state.integral == 0);
        ASSERT(state.differential == 0);
        ASSERT(state.total == 0);
        ASSERT(state.delta == 0);
        ASSERT(state.previous == 0);
        ASSERT(state.initialized == 0);

        ASSERT(diminuto_controller_fini(&parameters, &state) == (diminuto_controller_state_t *)0);

        STATUS();
    }

    {
        diminuto_controller_state_t state;

        TEST();

        memset(&state, 0x55, sizeof(state));

        /*
         * The state of the controller can be reset without affecting the
         * parameters.
         */

        ASSERT(diminuto_controller_init((diminuto_controller_parameters_t *)0, &state) == &state);

        diminuto_controller_state_print(stdout, &state);

        ASSERT(state.sample == 0);
        ASSERT(state.proportional == 0);
        ASSERT(state.integral == 0);
        ASSERT(state.differential == 0);
        ASSERT(state.total == 0);
        ASSERT(state.delta == 0);
        ASSERT(state.previous == 0);
        ASSERT(state.initialized == 0);

        ASSERT(diminuto_controller_fini((diminuto_controller_parameters_t *)0, &state) == (diminuto_controller_state_t *)0);

        STATUS();
    }

    {
        diminuto_controller_parameters_t parameters;

        TEST();

        memset(&parameters, 0xAA, sizeof(parameters));

        /*
         * The parameters can be returned to their defaults without
         * affecting the state of the controller.
         */

        ASSERT(diminuto_controller_init(&parameters, (diminuto_controller_state_t *)0) == (diminuto_controller_state_t *)0);

        diminuto_controller_parameters_print(stdout, &parameters);

        ASSERT(parameters.windup == DIMINUTO_CONTROLLER_MAXIMUM_VALUE);
        ASSERT(parameters.minimum == DIMINUTO_CONTROLLER_MINIMUM_OUTPUT);
        ASSERT(parameters.maximum == DIMINUTO_CONTROLLER_MAXIMUM_OUTPUT);
        ASSERT(parameters.lower == DIMINUTO_CONTROLLER_MINIMUM_OUTPUT);
        ASSERT(parameters.upper == DIMINUTO_CONTROLLER_MAXIMUM_OUTPUT);
        ASSERT(parameters.kp.numerator == 1);
        ASSERT(parameters.kp.denominator == 1);
        ASSERT(parameters.ki.numerator == 1);
        ASSERT(parameters.ki.denominator == 1);
        ASSERT(parameters.kd.numerator == 1);
        ASSERT(parameters.kd.denominator == 1);
        ASSERT(parameters.kc.numerator == 1);
        ASSERT(parameters.kc.denominator == 1);
        ASSERT(parameters.filter == !0);

        ASSERT(diminuto_controller_fini(&parameters, (diminuto_controller_state_t *)0) == (diminuto_controller_state_t *)0);

        STATUS();
    }

    {
        TEST();

        ASSERT(diminuto_controller_init((diminuto_controller_parameters_t *)0, (diminuto_controller_state_t *)0) == (diminuto_controller_state_t *)0);
        ASSERT(diminuto_controller_fini((diminuto_controller_parameters_t *)0, (diminuto_controller_state_t *)0) == (diminuto_controller_state_t *)0);

        STATUS();
    }

    {
        diminuto_controller_parameters_t parameters;
        diminuto_controller_state_t state;

        TEST();

        ASSERT(diminuto_controller_init(&parameters, &state) == &state);
        parameters.minimum = 0;
        parameters.lower = 0;

        diminuto_controller_parameters_print(stdout, &parameters);
        diminuto_controller_state_print(stdout, &state);

        fcontrol(&parameters, &state, 1000, 500, 1, 60);

        ASSERT(diminuto_controller_fini(&parameters, &state) == (diminuto_controller_state_t *)0);

        STATUS();
    }

    {
        diminuto_controller_parameters_t parameters;
        diminuto_controller_state_t state;

        TEST();

        ASSERT(diminuto_controller_init(&parameters, &state) == &state);
        parameters.minimum = 0;
        parameters.lower = 0;

        diminuto_controller_parameters_print(stdout, &parameters);
        diminuto_controller_state_print(stdout, &state);

        fcontrol(&parameters, &state, 1000, 2000, 1, 60);

        ASSERT(diminuto_controller_fini(&parameters, &state) == (diminuto_controller_state_t *)0);

        STATUS();
    }

    {
        diminuto_controller_parameters_t parameters;
        diminuto_controller_state_t state;

        TEST();

        ASSERT(diminuto_controller_init(&parameters, &state) == &state);
        parameters.minimum = 0;
        parameters.lower = 0;
        parameters.kp.numerator = 1;
        parameters.kp.denominator = 1;
        parameters.ki.numerator = 1;
        parameters.ki.denominator = 2;
        parameters.kd.numerator = 1;
        parameters.kd.denominator = 1;

        diminuto_controller_parameters_print(stdout, &parameters);
        diminuto_controller_state_print(stdout, &state);

        fcontrol(&parameters, &state, 1000, 2000, 1, 60);

        ASSERT(diminuto_controller_fini(&parameters, &state) == (diminuto_controller_state_t *)0);

        STATUS();
    }

    EXIT();
}
