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

#include <string.h>
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
        struct utsname buffer = { 0, };
        int rc;
        size_t ss;
        size_t ll;

        TEST();

        rc = diminuto_platform_system(&buffer);
        ASSERT(rc >= 0);

        {
            ASSERT(COM_DIAG_DIMINUTO_PLATFORM != (char *)0);
            ASSERT((ss = sizeof(COM_DIAG_DIMINUTO_PLATFORM)) > 0);
            ASSERT(COM_DIAG_DIMINUTO_PLATFORM[0] != '\0');
            ASSERT((ll = strlen(COM_DIAG_DIMINUTO_PLATFORM)) < ss);
            printf("COM_DIAG_DIMINUTO_PLATFORM[%zu][%zu]=\"%s\"\n", ll, ss,  COM_DIAG_DIMINUTO_PLATFORM);
        }

        {
            ASSERT(buffer.sysname != (char *)0);
            ASSERT((ss = sizeof(buffer.sysname)) > 0);
            ASSERT(buffer.sysname[0] != '\0');
            ASSERT((ll = strlen(buffer.sysname)) < ss);
            printf("sysname[%zu][%zu]=\"%s\"\n", ll, ss, buffer.sysname);
        }

        {
            ASSERT(buffer.nodename != (char *)0);
            ASSERT((ss = sizeof(buffer.nodename)) > 0);
            ASSERT(buffer.nodename[0] != '\0');
            ASSERT((ll = strlen(buffer.nodename)) < ss);
            printf("nodename[%zu][%zu]=\"%s\"\n", ll, ss, buffer.nodename);
        }

        {
            ASSERT(buffer.release != (char *)0);
            ASSERT((ss = sizeof(buffer.release)) > 0);
            ASSERT(buffer.release[0] != '\0');
            ASSERT((ll = strlen(buffer.release)) < ss);
            printf("release[%zu][%zu]=\"%s\"\n", ll, ss, buffer.release);
        }

        {
            ASSERT(buffer.version != (char *)0);
            ASSERT((ss = sizeof(buffer.version)) > 0);
            ASSERT(buffer.version[0] != '\0');
            ASSERT((ll = strlen(buffer.version)) < ss);
            printf("version[%zu][%zu]=\"%s\"\n", ll, ss, buffer.version);
        }

        {
            ASSERT(buffer.machine != (char *)0);
            ASSERT((ss = sizeof(buffer.machine)) > 0);
            ASSERT(buffer.machine[0] != '\0');
            ASSERT((ll = strlen(buffer.machine)) < ss);
            printf("machine[%zu][%zu]=\"%s\"\n", ll, ss, buffer.machine);
        }

#ifdef _GNU_SOURCE
        {
            ASSERT(buffer.domainname != (char *)0);
            ASSERT((ss = sizeof(buffer.domainname)) > 0);
            ASSERT(buffer.domainname[0] != '\0');
            ASSERT((ll = strlen(buffer.domainname)) < ss);
            printf("domainname[%zu][%zu]=\"%s\"\n", ll, ss, buffer.domainname);
        }
#endif

        STATUS();
	}

    EXIT();
}
