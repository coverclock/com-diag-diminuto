/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Condition feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Condition feature.
 */

#include "com/diag/diminuto/diminuto_condition.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <pthread.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

int main(void)
{
    int rc;
    diminuto_condition_t condition = DIMINUTO_CONDITION_INITIALIZER;
    diminuto_ticks_t ticks;

    diminuto_log_setmask();

    {
        TEST();

        ASSERT(DIMINUTO_CONDITION_INFINITY == (~(diminuto_ticks_t)0));
        ASSERT((int)DIMINUTO_CONDITION_ERROR ==  (int)DIMINUTO_MUTEX_ERROR);
        ASSERT(DIMINUTO_CONDITION_TIMEDOUT == ETIMEDOUT);

        ASSERT(COM_DIAG_DIMINUTO_CONDITION_FREQUENCY == 1000000000LL);
        ASSERT(diminuto_condition_frequency() == COM_DIAG_DIMINUTO_CONDITION_FREQUENCY);

        STATUS();
    }

    {
        TEST();

        ASSERT(diminuto_condition_init(&condition) == &condition);

        STATUS();
    }

    {
        TEST();

        DIMINUTO_CONDITION_BEGIN(&condition);
            ticks = 0;
            COMMENT("PAUSING epoch");
            rc = diminuto_condition_wait_until(&condition, ticks);
            ASSERT(rc == DIMINUTO_CONDITION_TIMEDOUT);
        DIMINUTO_CONDITION_END;

        STATUS();
    }

    {
        TEST();

        DIMINUTO_CONDITION_BEGIN(&condition);
            ticks = diminuto_condition_clock() - diminuto_frequency();
            COMMENT("PAUSING earlier");
            rc = diminuto_condition_wait_until(&condition, ticks);
            ASSERT(rc == DIMINUTO_CONDITION_TIMEDOUT);
        DIMINUTO_CONDITION_END;

        STATUS();
    }

    {
        TEST();

        DIMINUTO_CONDITION_BEGIN(&condition);
            ticks = diminuto_condition_clock();
            COMMENT("PAUSING now");
            rc = diminuto_condition_wait_until(&condition, ticks);
            ASSERT(rc == DIMINUTO_CONDITION_TIMEDOUT);
        DIMINUTO_CONDITION_END;

        STATUS();
    }

    {
        TEST();

        DIMINUTO_CONDITION_BEGIN(&condition);
            ticks = diminuto_condition_clock() + diminuto_frequency();
            COMMENT("PAUSING later");
            rc = diminuto_condition_wait_until(&condition, ticks);
            ASSERT(rc == DIMINUTO_CONDITION_TIMEDOUT);
        DIMINUTO_CONDITION_END;

        STATUS();
    }

    {
        TEST();

        DIMINUTO_CONDITION_BEGIN(&condition);
            ticks = diminuto_condition_clock() + (10 * diminuto_frequency());
            COMMENT("PAUSING much later");
            rc = diminuto_condition_wait_until(&condition, ticks);
            ASSERT(rc == DIMINUTO_CONDITION_TIMEDOUT);
        DIMINUTO_CONDITION_END;

        STATUS();
    }

    EXIT();
}
