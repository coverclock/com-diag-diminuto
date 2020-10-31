/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2019 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Observation feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Observation feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_observation.h"
#include "com/diag/diminuto/diminuto_time.h"
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
    uint64_t sn = 0;

    SETLOGMASK();

    errno = 0;

    {
        const char * PATH = "/tmp/diminuto-unittest-observation-1";
        char * temp = (char *)0;
        FILE * fp = (FILE *)0;
        uint64_t expected = 0;
        uint64_t actual = 0;
        int rc = -1;
        struct stat status = { 0, };
        size_t count = 0;
        char * copy = (char *)0;

        TEST();

        fp = diminuto_observation_create(PATH, &temp);
        ASSERT(fp != (FILE *)0);
        ASSERT(temp != (const char *)0);
        COMMENT("PATH=\"%s\"[%zu] temp=\"%s\"[%zu] fp=%p\n", PATH, strlen(PATH), temp, strlen(temp), fp);
        ASSERT(strlen(temp) == (strlen(PATH) + strlen("-XXXXXX")));
        ASSERT(strncmp(temp, PATH, strlen(PATH)) == 0);

        rc = stat(temp, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o\n", status.st_mode);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        expected = sn++;
        ASSERT(expected >= 0);

        count = fwrite(&expected, sizeof(expected), 1, fp);
        ASSERT(count == 1);

        copy = strdup(temp);
        ASSERT(copy != (char *)0);

        fp = diminuto_observation_commit(fp, &temp);
        ASSERT(fp == (FILE *)0);
        ASSERT(temp == (const char *)0);

        rc = stat(copy, &status);
        ASSERT(rc < 0);
        free(copy);

        rc = stat(PATH, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o size=%zu\n", status.st_mode, status.st_size);
        ASSERT(status.st_size == sizeof(expected));

        fp = fopen(PATH, "r");
        ASSERT(fp != (FILE *)0);

        count = fread(&actual, sizeof(actual), 1, fp);
        ASSERT(count == 1);
        COMMENT("expected=%d actual=%d\n", expected, actual);
        ASSERT(actual == expected);

        rc = fclose(fp);
        ASSERT(rc == 0);

        rc = unlink(PATH);
        ASSERT(rc == 0);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        STATUS();
    }

    {
        const char * PATH = "diminuto-unittest-observation-2";
        char * temp = (char *)0;
        FILE * fp = (FILE *)0;
        long int expected = 0;
        long int actual = 0;
        int rc = -1;
        struct stat status = { 0, };
        size_t count = 0;
        char * copy = (char *)0;

        TEST();

        fp = diminuto_observation_create(PATH, &temp);
        ASSERT(fp != (FILE *)0);
        ASSERT(temp != (const char *)0);
        COMMENT("PATH=\"%s\"[%zu] temp=\"%s\"[%zu] fp=%p\n", PATH, strlen(PATH), temp, strlen(temp), fp);
        ASSERT(strlen(temp) == (strlen(PATH) + strlen("-XXXXXX")));
        ASSERT(strncmp(temp, PATH, strlen(PATH)) == 0);

        rc = stat(temp, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o\n", status.st_mode);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        expected = sn++;
        ASSERT(expected >= 0);

        count = fwrite(&expected, sizeof(expected), 1, fp);
        ASSERT(count == 1);

        copy = strdup(temp);
        ASSERT(copy != (char *)0);

        fp = diminuto_observation_discard(fp, &temp);
        ASSERT(fp == (FILE *)0);
        ASSERT(temp == (char *)0);

        rc = stat(copy, &status);
        ASSERT(rc < 0);
        free(copy);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        STATUS();
    }

    {
        const char * PATH = "/tmp/diminuto-unittest-observation";
        char * temp = (char *)0;
        FILE * fp = (FILE *)0;
        long int expected = 0;
        long int actual = 0;
        int rc = -1;
        struct stat status = { 0, };
        size_t count = 0;
        char * copy = (char *)0;

        TEST();

        /* FIRST */

        fp = diminuto_observation_create(PATH, &temp);
        ASSERT(fp != (FILE *)0);
        ASSERT(temp != (const char *)0);
        COMMENT("PATH=\"%s\"[%zu] temp=\"%s\"[%zu] fp=%p\n", PATH, strlen(PATH), temp, strlen(temp), fp);
        ASSERT(strlen(temp) == (strlen(PATH) + strlen("-XXXXXX")));
        ASSERT(strncmp(temp, PATH, strlen(PATH)) == 0);

        rc = stat(temp, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o\n", status.st_mode);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        expected = sn++;
        ASSERT(expected >= 0);

        count = fwrite(&expected, sizeof(expected), 1, fp);
        ASSERT(count == 1);

        copy = strdup(temp);
        ASSERT(copy != (char *)0);

        fp = diminuto_observation_commit(fp, &temp);
        ASSERT(fp == (FILE *)0);
        ASSERT(temp == (char *)0);

        rc = stat(copy, &status);
        ASSERT(rc < 0);
        free(copy);

        rc = stat(PATH, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o size=%zu\n", status.st_mode, status.st_size);
        ASSERT(status.st_size == sizeof(expected));

        fp = fopen(PATH, "r");
        ASSERT(fp != (FILE *)0);

        count = fread(&actual, sizeof(actual), 1, fp);
        ASSERT(count == 1);
        COMMENT("expected=%d actual=%d\n", expected, actual);
        ASSERT(actual == expected);

        rc = fclose(fp);
        ASSERT(rc == 0);

        /* SECOND */

        fp = diminuto_observation_create(PATH, &temp);
        ASSERT(fp != (FILE *)0);
        ASSERT(temp != (const char *)0);
        COMMENT("PATH=\"%s\"[%zu] temp=\"%s\"[%zu] fp=%p\n", PATH, strlen(PATH), temp, strlen(temp), fp);
        ASSERT(strlen(temp) == (strlen(PATH) + strlen("-XXXXXX")));
        ASSERT(strncmp(temp, PATH, strlen(PATH)) == 0);

        rc = stat(temp, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o\n", status.st_mode);

        rc = stat(PATH, &status);
        ASSERT(rc >= 0);

        expected = sn++;

        count = fwrite(&expected, sizeof(expected), 1, fp);
        ASSERT(count == 1);

        copy = strdup(temp);
        ASSERT(copy != (char *)0);

        fp = diminuto_observation_commit(fp, &temp);
        ASSERT(fp == (FILE *)0);
        ASSERT(temp == (char *)0);

        rc = stat(copy, &status);
        ASSERT(rc < 0);
        free(copy);

        rc = stat(PATH, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o size=%zu\n", status.st_mode, status.st_size);
        ASSERT(status.st_size == sizeof(expected));

        fp = fopen(PATH, "r");
        ASSERT(fp != (FILE *)0);

        count = fread(&actual, sizeof(actual), 1, fp);
        ASSERT(count == 1);
        COMMENT("expected=%d actual=%d\n", expected, actual);
        ASSERT(actual == expected);

        rc = fclose(fp);
        ASSERT(rc == 0);

        /* CLEANUP */

        rc = unlink(PATH);
        ASSERT(rc == 0);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        STATUS();
    }

    {
        const char * PATH = "/tmp/diminuto-unittest-observation-3";
        char * temp = (char *)0;
        FILE * fp = (FILE *)0;
        long int expected = 0;
        long int actual = 0;
        int rc = -1;
        struct stat status = { 0, };
        size_t count = 0;
        char * copy = (char *)0;

        TEST();

        /* FIRST */

        fp = diminuto_observation_create(PATH, &temp);
        ASSERT(fp != (FILE *)0);
        ASSERT(temp != (const char *)0);
        COMMENT("PATH=\"%s\"[%zu] temp=\"%s\"[%zu] fp=%p\n", PATH, strlen(PATH), temp, strlen(temp), fp);
        ASSERT(strlen(temp) == (strlen(PATH) + strlen("-XXXXXX")));
        ASSERT(strncmp(temp, PATH, strlen(PATH)) == 0);

        rc = stat(temp, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o\n", status.st_mode);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        expected = sn++;
        ASSERT(expected >= 0);

        count = fwrite(&expected, sizeof(expected), 1, fp);
        ASSERT(count == 1);

        copy = strdup(temp);
        ASSERT(copy != (char *)0);

        fp = diminuto_observation_checkpoint(fp, &temp);
        ASSERT(fp != (FILE *)0);
        ASSERT(temp != (char *)0);

        fp = diminuto_observation_commit(fp, &temp);
        ASSERT(fp == (FILE *)0);
        ASSERT(temp == (char *)0);

        rc = stat(copy, &status);
        ASSERT(rc < 0);
        free(copy);

        rc = stat(PATH, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o size=%zu\n", status.st_mode, status.st_size);
        ASSERT(status.st_size == sizeof(expected));

        fp = fopen(PATH, "r");
        ASSERT(fp != (FILE *)0);

        count = fread(&actual, sizeof(actual), 1, fp);
        ASSERT(count == 1);
        COMMENT("expected=%d actual=%d\n", expected, actual);
        ASSERT(actual == expected);

        rc = fclose(fp);
        ASSERT(rc == 0);

        /* SECOND */

        fp = diminuto_observation_create(PATH, &temp);
        ASSERT(fp != (FILE *)0);
        ASSERT(temp != (const char *)0);
        COMMENT("PATH=\"%s\"[%zu] temp=\"%s\"[%zu] fp=%p\n", PATH, strlen(PATH), temp, strlen(temp), fp);
        ASSERT(strlen(temp) == (strlen(PATH) + strlen("-XXXXXX")));
        ASSERT(strncmp(temp, PATH, strlen(PATH)) == 0);

        rc = stat(temp, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o\n", status.st_mode);

        rc = stat(PATH, &status);
        ASSERT(rc >= 0);

        expected = sn++;

        count = fwrite(&expected, sizeof(expected), 1, fp);
        ASSERT(count == 1);

        copy = strdup(temp);
        ASSERT(copy != (char *)0);

        fp = diminuto_observation_checkpoint(fp, &temp);
        ASSERT(fp != (FILE *)0);
        ASSERT(temp != (char *)0);

        fp = diminuto_observation_commit(fp, &temp);
        ASSERT(fp == (FILE *)0);
        ASSERT(temp == (char *)0);

        rc = stat(copy, &status);
        ASSERT(rc < 0);
        free(copy);

        rc = stat(PATH, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o size=%zu\n", status.st_mode, status.st_size);
        ASSERT(status.st_size == sizeof(expected));

        fp = fopen(PATH, "r");
        ASSERT(fp != (FILE *)0);

        count = fread(&actual, sizeof(actual), 1, fp);
        ASSERT(count == 1);
        COMMENT("expected=%d actual=%d\n", expected, actual);
        ASSERT(actual == expected);

        rc = fclose(fp);
        ASSERT(rc == 0);

        /* CLEANUP */

        rc = unlink(PATH);
        ASSERT(rc == 0);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        STATUS();
    }

    /*
     * It's a good idea to run this with valgrind(3) to insure that the
     * feature is releasing memory as promised.
     */

    EXIT();

}
