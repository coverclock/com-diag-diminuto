/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2019 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_observation.h"
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{

    SETLOGMASK();

    errno = 0;

    {
        const char * PATH = "/tmp/diminuto-unittest-observation";
        char * temp = (char *)0;
        FILE * fp = (FILE *)0;
        pid_t pid = 0;
        pid_t datum = 0;
        int rc = -1;
        struct stat status = { 0, };
        size_t count = 0;
        char * copy = (char *)0;

        TEST();

        fp = diminuto_observation_create(PATH, &temp);
        ASSERT(fp != (FILE *)0);
        ASSERT(temp != (const char *)0);
        COMMENT("PATH=\"%s\"[%zu] temp=\"%s\"[%zu] fp=%p\n", PATH, strlen(PATH), temp, strlen(temp), fp);
        ASSERT(strlen(temp) == (strlen(PATH) + strlen("XXXXXX")));
        ASSERT(strncmp(temp, PATH, strlen(PATH)) == 0);

        rc = stat(temp, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o\n", status.st_mode);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        pid = getpid();
        ASSERT(pid > 0);

        count = fwrite(&pid, sizeof(pid), 1, fp);
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
        ASSERT(status.st_size == sizeof(pid));

        fp = fopen(PATH, "r");
        ASSERT(fp != (FILE *)0);

        count = fread(&datum, sizeof(datum), 1, fp);
        ASSERT(count == 1);
        COMMENT("pid=%d datum=%d\n", pid, datum);
        ASSERT(datum == pid);

        rc = fclose(fp);
        ASSERT(rc == 0);

        rc = unlink(PATH);
        ASSERT(rc == 0);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        STATUS();
    }

    {
        const char * PATH = "diminuto-unittest-observation";
        char * temp = (char *)0;
        FILE * fp = (FILE *)0;
        pid_t pid = 0;
        pid_t datum = 0;
        int rc = -1;
        struct stat status = { 0, };
        size_t count = 0;
        char * copy = (char *)0;

        TEST();

        fp = diminuto_observation_create(PATH, &temp);
        ASSERT(fp != (FILE *)0);
        ASSERT(temp != (const char *)0);
        COMMENT("PATH=\"%s\"[%zu] temp=\"%s\"[%zu] fp=%p\n", PATH, strlen(PATH), temp, strlen(temp), fp);
        ASSERT(strlen(temp) == (strlen(PATH) + strlen("XXXXXX")));
        ASSERT(strncmp(temp, PATH, strlen(PATH)) == 0);

        rc = stat(temp, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o\n", status.st_mode);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        pid = getpid();
        ASSERT(pid > 0);

        count = fwrite(&pid, sizeof(pid), 1, fp);
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
        pid_t pid = 0;
        pid_t datum = 0;
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
        ASSERT(strlen(temp) == (strlen(PATH) + strlen("XXXXXX")));
        ASSERT(strncmp(temp, PATH, strlen(PATH)) == 0);

        rc = stat(temp, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o\n", status.st_mode);

        rc = stat(PATH, &status);
        ASSERT(rc < 0);

        pid = getpid();
        ASSERT(pid > 0);

        count = fwrite(&pid, sizeof(pid), 1, fp);
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
        ASSERT(status.st_size == sizeof(pid));

        fp = fopen(PATH, "r");
        ASSERT(fp != (FILE *)0);

        count = fread(&datum, sizeof(datum), 1, fp);
        ASSERT(count == 1);
        COMMENT("pid=%d datum=%d\n", pid, datum);
        ASSERT(datum == pid);

        rc = fclose(fp);
        ASSERT(rc == 0);

        /* SECOND */

        fp = diminuto_observation_create(PATH, &temp);
        ASSERT(fp != (FILE *)0);
        ASSERT(temp != (const char *)0);
        COMMENT("PATH=\"%s\"[%zu] temp=\"%s\"[%zu] fp=%p\n", PATH, strlen(PATH), temp, strlen(temp), fp);
        ASSERT(strlen(temp) == (strlen(PATH) + strlen("XXXXXX")));
        ASSERT(strncmp(temp, PATH, strlen(PATH)) == 0);

        rc = stat(temp, &status);
        ASSERT(rc >= 0);
        COMMENT("mode=0%o\n", status.st_mode);

        rc = stat(PATH, &status);
        ASSERT(rc >= 0);

        pid += 1;

        count = fwrite(&pid, sizeof(pid), 1, fp);
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
        ASSERT(status.st_size == sizeof(pid));

        fp = fopen(PATH, "r");
        ASSERT(fp != (FILE *)0);

        count = fread(&datum, sizeof(datum), 1, fp);
        ASSERT(count == 1);
        COMMENT("pid=%d datum=%d\n", pid, datum);
        ASSERT(datum == pid);

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
