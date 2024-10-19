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
#include "diminuto_realtime.h"

int main(int argc, char * argv[])
{
    SETLOGMASK();

    {
        int rc;
        FILE * fp;

        TEST();

        rc = diminuto_realtime_supported_path((const char *)0);
        ASSERT(rc < 0);
        EXPECT(errno == EINVAL);

        rc = diminuto_realtime_supported_path("/proc/1/mem");
        ASSERT(rc < 0);
        EXPECT(errno == EACCES);

        rc = diminuto_realtime_supported_path("./DOES-NOT-EXIST");
        ASSERT(rc == 0);

        ASSERT((fp = tmpfile()) != (FILE *)0);
        ASSERT((rc = fprintf(fp, "%d\n", -1)) > 0);
        rc = diminuto_realtime_supported_fp(fp, "TMP1");
        ASSERT(rc == 0);

        ASSERT((fp = tmpfile()) != (FILE *)0);
        ASSERT((rc = fprintf(fp, "%d\n", 0)) > 0);
        rc = diminuto_realtime_supported_fp(fp, "TMP2");
        ASSERT(rc == 0);

        ASSERT((fp = tmpfile()) != (FILE *)0);
        ASSERT((rc = fprintf(fp, "%d\n", 1)) > 0);
        rc = diminuto_realtime_supported_fp(fp, "TMP3");
        ASSERT(rc > 0);

        rc = diminuto_realtime_supported();
        ASSERT(rc >= 0);

        NOTIFY("RT_EXEMPT is %ssupported.\n", rc ? "" : "not ");

        STATUS();
    }

    EXIT();
}
