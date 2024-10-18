/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the unit test for the Real Time feature supporting RT_PREEMPT>
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the unit test for the Real Time feature supporting RT_PREEMPT>
 */

#include "com/diag/diminuto/diminuto_realtime.h"
#include "com/diag/diminuto/diminuto_unittest.h"

int main(int argc, char * argv[])
{
    SETLOGMASK();

    {
        int rc;

        TEST();

        rc = diminuto_realtime_supported_path((const char *)0);
        ASSERT(rc < 0);

        rc = diminuto_realtime_supported_path("/tmp/DOES-NOT-EXIST");
        ASSERT(rc == 0);

        rc = diminuto_realtime_supported_path("/proc/1/mem");
        ASSERT(rc < 0);

        rc = diminuto_realtime_supported();
        ASSERT(rc >= 0);

        NOTIFY("RT_EXEMPT is %ssupported\n", rc ? "" : "not ");

        STATUS();
    }

    EXIT();
}
