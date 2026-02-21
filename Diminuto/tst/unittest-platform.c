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
        static const size_t LIMIT = 65;
        struct utsname buffer = { 0, };
        int rc;
        size_t ss;

        TEST();

        rc = diminuto_platform_system(&buffer);
        ASSERT(rc >= 0);

        {
            ASSERT(COM_DIAG_DIMINUTO_PLATFORM != (char *)0);
            ASSERT(sizeof(COM_DIAG_DIMINUTO_PLATFORM) > 0);
            ASSERT(COM_DIAG_DIMINUTO_PLATFORM[0] != '\0');
            ASSERT((ss = strlen(COM_DIAG_DIMINUTO_PLATFORM)) < LIMIT);
            printf("COM_DIAG_DIMINUTO_PLATFORM[%zu][%zu]=\"%s\"\n", sizeof(COM_DIAG_DIMINUTO_PLATFORM), ss,  COM_DIAG_DIMINUTO_PLATFORM);
        }

        {
            ASSERT(buffer.sysname != (char *)0);
            ASSERT(sizeof(buffer.sysname) > 0);
            ASSERT(buffer.sysname[0] != '\0');
            ASSERT((ss = strlen(buffer.sysname)) < LIMIT);
            printf("sysname[%zu][%zu]=\"%s\"\n", sizeof(buffer.sysname), ss, buffer.sysname);
        }

        {
            ASSERT(buffer.nodename != (char *)0);
            ASSERT(sizeof(buffer.nodename) > 0);
            ASSERT(buffer.nodename[0] != '\0');
            ASSERT((ss = strlen(buffer.nodename)) < LIMIT);
            printf("nodename[%zu][%zu]=\"%s\"\n", sizeof(buffer.nodename), ss, buffer.nodename);
        }

        {
            ASSERT(buffer.release != (char *)0);
            ASSERT(sizeof(buffer.release) > 0);
            ASSERT(buffer.release[0] != '\0');
            ASSERT((ss = strlen(buffer.release)) < LIMIT);
            printf("release[%zu][%zu]=\"%s\"\n", sizeof(buffer.release), ss, buffer.release);
        }

        {
            ASSERT(buffer.version != (char *)0);
            ASSERT(sizeof(buffer.version) > 0);
            ASSERT(buffer.version[0] != '\0');
            ASSERT((ss = strlen(buffer.version)) < LIMIT);
            printf("version[%zu][%zu]=\"%s\"\n", sizeof(buffer.version), ss, buffer.version);
        }

        {
            ASSERT(buffer.machine != (char *)0);
            ASSERT(sizeof(buffer.machine) > 0);
            ASSERT(buffer.machine[0] != '\0');
            ASSERT((ss = strlen(buffer.machine)) < LIMIT);
            printf("machine[%zu][%zu]=\"%s\"\n", sizeof(buffer.machine), ss, buffer.machine);
        }

#ifdef _GNU_SOURCE
        {
            ASSERT(buffer.domainname != (char *)0);
            ASSERT(sizeof(buffer.domainname) > 0);
            ASSERT(buffer.domainname[0] != '\0');
            ASSERT((ss = strlen(buffer.domainname)) < LIMIT);
            printf("domainname[%zu][%zu]=\"%s\"\n", sizeof(buffer.domainname), ss, buffer.domainname);
        }
#endif

        STATUS();
	}

    EXIT();
}
