/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
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
    diminuto_log_setmask();

    {
        diminuto_condition_t condition = DIMINUTO_CONDITION_INITIALIZER;
    }

    {
        diminuto_condition_t condition;
        ASSERT(diminuto_condition_init(&condition) == &condition);
    }

    {
        int rc;
        diminuto_condition_t condition;
        diminuto_ticks_t ticks;

        TEST();

        diminuto_condition_init(&condition);

        DIMINUTO_CONDITION_BEGIN(&condition);
            COMMENT("PAUSING -s");
            rc = diminuto_condition_wait_until(&condition, 0);
            ASSERT(rc == DIMINUTO_CONDITION_TIMEDOUT);
        DIMINUTO_CONDITION_END;

        DIMINUTO_CONDITION_BEGIN(&condition);
            ticks = diminuto_condition_clock();
            COMMENT("PAUSING 0s");
            rc = diminuto_condition_wait_until(&condition, ticks);
            ASSERT(rc == DIMINUTO_CONDITION_TIMEDOUT);
        DIMINUTO_CONDITION_END;

        DIMINUTO_CONDITION_BEGIN(&condition);
            ticks = diminuto_condition_clock() + (diminuto_frequency() * 5);
            COMMENT("PAUSING 5s");
            rc = diminuto_condition_wait_until(&condition, ticks);
            ASSERT(rc == DIMINUTO_CONDITION_TIMEDOUT);
        DIMINUTO_CONDITION_END;

        STATUS();
    }

    EXIT();
}
