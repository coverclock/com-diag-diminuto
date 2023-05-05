/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the version info.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the version info.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_version.h"

int main(void)
{
    SETLOGMASK();

    {
        TEST();

        ASSERT(COM_DIAG_DIMINUTO_PLATFORM_VALUE != (const char *)0);
        ASSERT(COM_DIAG_DIMINUTO_RELEASE_VALUE != (const char *)0);
        ASSERT(COM_DIAG_DIMINUTO_REVISION_VALUE != (const char *)0);
        ASSERT(COM_DIAG_DIMINUTO_VINTAGE_VALUE != (const char *)0);

        CHECKPOINT("PLATFORM=\"%s\"\n", COM_DIAG_DIMINUTO_PLATFORM_VALUE);
        CHECKPOINT("RELEASE=\"%s\"\n", COM_DIAG_DIMINUTO_RELEASE_VALUE);
        CHECKPOINT("REVISION=\"%s\"\n", COM_DIAG_DIMINUTO_REVISION_VALUE);
        CHECKPOINT("VINTAGE=\"%s\"\n", COM_DIAG_DIMINUTO_VINTAGE_VALUE);

        STATUS();
	}

    EXIT();
}
