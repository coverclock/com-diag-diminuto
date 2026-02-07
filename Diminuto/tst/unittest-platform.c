/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2026 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the platform info.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the plaform info.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_platform.h"

int main(void)
{
    SETLOGMASK();

    {
        int rc;

        TEST();

        rc = diminuto_platform_system((struct utsname *)0);
        ASSERT(rc < 0);

        STATUS();
	}

    {
        struct utsname buffer;
        int rc;

        TEST();

        rc = diminuto_platform_system(&buffer);
        ASSERT(rc >= 0);

        printf("COM_DIAG_DIMINUTO_PLATFORM=\"%s\"\n", COM_DIAG_DIMINUTO_PLATFORM);
        printf("sysname[%zu]=\"%s\"\n", sizeof(buffer.sysname), buffer.sysname);
        printf("nodename[%zu]=\"%s\"\n", sizeof(buffer.nodename), buffer.nodename);
        printf("release[%zu]=\"%s\"\n", sizeof(buffer.release), buffer.release);
        printf("version[%zu]=\"%s\"\n", sizeof(buffer.version), buffer.version);
        printf("machine[%zu]=\"%s\"\n", sizeof(buffer.machine), buffer.machine);
#ifdef _GNU_SOURCE
        printf("domainname[%zu]=\"%s\"\n", sizeof(buffer.domainname), buffer.domainname);
#endif

        STATUS();
	}

    EXIT();
}
