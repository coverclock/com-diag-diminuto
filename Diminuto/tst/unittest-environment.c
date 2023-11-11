/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Environment feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @details
 * This is a unit test of the Environment feature.
 * It currently does not specifically test the reader/writer locking.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_environment.h"
#include <string.h>

int main(void)
{
    SETLOGMASK();

    {
        const char NAME[] = "COM_DIAG_DIMINUTO_ENVIRONMENT_TEST";
        char STRING[] = "COM_DIAG_DIMINUTO_ENVIRONMENT_TEST=1";
        const char * value;
        int rc;

        TEST();

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        DIMINUTO_ENVIRONMENT_READER_BEGIN;
            ASSERT(diminuto_environment_readerwriter.waiting == 0);
            ASSERT(diminuto_environment_readerwriter.reading == 1);
            ASSERT(diminuto_environment_readerwriter.writing == 0);
        DIMINUTO_ENVIRONMENT_READER_END;

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        DIMINUTO_ENVIRONMENT_WRITER_BEGIN;
            ASSERT(diminuto_environment_readerwriter.waiting == 0);
            ASSERT(diminuto_environment_readerwriter.reading == 0);
            ASSERT(diminuto_environment_readerwriter.writing == 1);
        DIMINUTO_ENVIRONMENT_WRITER_END;

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        value = diminuto_getenv(NAME);
        ASSERT(value == (const char *)0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        rc = diminuto_unsetenv(NAME);
        ASSERT(rc == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        rc = diminuto_putenv(STRING);
        ASSERT(rc == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        value = diminuto_getenv(NAME);
        ASSERT(value != (const char *)0);
        ASSERT(strcmp(value, "1") == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        rc = diminuto_setenv(NAME, "2", 0);
        ASSERT(rc == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        value = diminuto_getenv(NAME);
        ASSERT(value != (const char *)0);
        ASSERT(strcmp(value, "1") == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        rc = diminuto_setenv(NAME, "3", !0);
        ASSERT(rc == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        value = diminuto_getenv(NAME);
        ASSERT(value != (const char *)0);
        ASSERT(strcmp(value, "3") == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        rc = diminuto_unsetenv(NAME);
        ASSERT(rc == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        value = diminuto_getenv(NAME);
        ASSERT(value == (const char *)0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        rc = diminuto_setenv(NAME, "4", 0);
        ASSERT(rc == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        value = diminuto_getenv(NAME);
        ASSERT(value != (const char *)0);
        ASSERT(strcmp(value, "4") == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        rc = diminuto_putenv(STRING);
        ASSERT(rc == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        value = diminuto_getenv(NAME);
        ASSERT(value != (const char *)0);
        ASSERT(strcmp(value, "1") == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        rc = diminuto_unsetenv(NAME);
        ASSERT(rc == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        value = diminuto_getenv(NAME);
        ASSERT(value == (const char *)0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        rc = diminuto_unsetenv(NAME);
        ASSERT(rc == 0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        value = diminuto_getenv(NAME);
        ASSERT(value == (const char *)0);

        ASSERT(diminuto_environment_readerwriter.waiting == 0);
        ASSERT(diminuto_environment_readerwriter.reading == 0);
        ASSERT(diminuto_environment_readerwriter.writing == 0);

        STATUS();
    }

    EXIT();
}

